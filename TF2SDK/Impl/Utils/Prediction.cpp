
#include "Engine/ClientDll.hpp"
#include "Engine/NetChannel.hpp"
#include "Engine/GlobalVars.hpp"
#include "Engine/Convar.hpp"

#include "Client/Prediction.hpp"

#include "Entity/BasePlayer.hpp"

#include "Utils/Prediction.hpp"
#include "Utils/Trace.hpp"
#include "Utils/Vector.hpp"

TF2_NAMESPACE_BEGIN(::Utils);

static TF2::ConVar* cvar_sv_cheats();
static TF2::ConVar* cvar_cl_latency();


Vector3D_F Prediction_SimpleLatency(const Vector3D_F& position, const Vector3D_F& velocity)
{
	INetChannel* pNet = Interfaces::EngineClient->GetNetChannelInfo();
	float latency = pNet->GetLatency(Const::NetMsgFlowType::Outgoing) + pNet->GetLatency(Const::NetMsgFlowType::Incoming);
	return latency ? Vector3D_F{ } : Vector3D_F{ position + velocity * latency };
}



static Vector3D_F Prediction_NextStep(const Vector3D_F& pos, Vector3D_F& velocity, const Vector3D_F& acceleration, float step_size, const Vector3D_F(&minmax)[2], bool on_ground)
{
    Vector3D_F target_pos = pos + ((acceleration / 2) * std::pow(step_size, 2) + velocity * step_size);
    velocity += acceleration * step_size;
    float dist_to_ground = 0.f;

    // Check visibility
    {
        Utils::FilterWorldOnly filter_world_only;
        GameTrace trace;
        // If we are slightly above ground
        Utils::TraceHull({ target_pos[0], target_pos[1], target_pos[2] + 18.f }, target_pos, minmax[0], minmax[1], Const::TraceMask::All, &trace, &filter_world_only);

        if (trace.DidHit() && !trace.StartSolid)
        {
            target_pos = trace.EndPos;
        }
        // check distance between entity and ground
        else
        {
            Vector3D_F begin_pos{ target_pos[0], target_pos[1], target_pos[2] + 83.f };
            Vector3D_F end_pos{ begin_pos[0], begin_pos[1], static_cast<float>(-Const::MaxTrace_Length) };

            Utils::TraceHull(begin_pos, end_pos, minmax[0], minmax[1], Const::TraceMask::All, &trace, &filter_world_only);
            if (trace.Fraction != -1.0 && (trace.EndPos[2] - pos[2]) <= 18.f)
            {
                dist_to_ground = trace.EndPos[2] - target_pos[2];
                target_pos[2] = trace.EndPos[2];
            }


            Utils::TraceHull(pos, target_pos, minmax[0], minmax[1], Const::TraceMask::All, &trace, &filter_world_only);
            if (trace.DidHit() && trace.Plane.Normal.is_zero(0.0001f))
            {
                Vector3D_F hitpos = trace.EndPos;
                Vector3D_F normal_wall = trace.Plane.Normal;
                Vector3D_F normal_trace{ hitpos[0] - pos[0], hitpos[1] - pos[1], 0.f };
                normal_trace.normalize();

                // Angle of impact determines speed

                // We can get a plane in Normal form and determine the direction from there
                // aka n1*x+n2*y+n3*z=d
                float d = static_cast<float>(normal_wall.dot(hitpos));

                Vector3D_F pt1, pt2;

                // The above will be invalid due to a division by 0
                if (!normal_wall[2])
                {
                    pt1 = { (d - normal_wall[1] * velocity[1] - normal_wall[2] * hitpos[2]) / normal_wall[0], velocity[1], 0.f };
                    pt2 = { (d - normal_wall[1] * 2.f * velocity[1] - normal_wall[2] * hitpos[2]) / normal_wall[0], velocity[1] * 2.f, 0.f };
                }
                else
                {
                    pt1 = { velocity[1], (d - normal_wall[0] * velocity[0] - normal_wall[2] * hitpos[2]) / normal_wall[1], 0.f };
                    pt2 = { velocity[1] * 2.f, (d - normal_wall[0] * velocity[0] * 2.f - normal_wall[2] * hitpos[2]) / normal_wall[1], 0.f};
                }

                hitpos += normal_wall * velocity * step_size;
                target_pos = hitpos;

                // Adjust new velocity
                Vector3D_F new_vel = pt2 - pt1;

                // Ensure we have no 0 length
                if (double new_vel_len = new_vel.length())
                {
                    float impact_angle = static_cast<float>(acos(normal_trace.dot(normal_wall)));
                    // Adjust velocity depending on angle
                    double speed = Vector3D_F{ velocity[0], velocity[1], 0.f }.length() * (std::numbers::pi_v<float> - impact_angle);

                    new_vel /= new_vel_len;
                    new_vel = (new_vel / new_vel_len) * speed;
                    velocity[0] = new_vel[0];
                    velocity[1] = new_vel[1];
                }
            }
        }
    }

    if (on_ground)
        target_pos[2] -= dist_to_ground;
    return target_pos;
}


PredictionProjectileRet Prediction_Projectile(IBaseEntityInternal* pEnt, const Vector3D_F& position, float speed, float projectile_gravity, float ent_gravity, float proj_startvelocity)
{
	Vector3D_F origin = pEnt->VecOrigin;
    Vector3D_F velocity; pEnt->EstimateAbsVelocity(velocity);
    Vector3D_F acceleration{ 0.f, 0.f, cvar_sv_cheats()->FloatValue * -ent_gravity };
    const Vector3D_F minmax[]{
        pEnt->Mins,
        pEnt->Maxs
    };

    const Vector3D_F origin_to_pos = position - origin;
    Vector3D_F last_pos = origin;
    Vector3D_F best_pos = origin;

    const Vector3D_F local_eye_pos = ILocalPlayer{}->EyePosition();
    const float midpoint = static_cast<float>(local_eye_pos.distance_to(position) / speed);

    float current_time = std::max(midpoint - 1.5f, 0.01f);
    const float first_time = current_time;
    float best_time = current_time;
    float min_delta = static_cast<float>(std::numeric_limits<uint16_t>::max());

    const bool on_ground = pEnt->IsClassID(Const::EntClassID::CTFPlayer) && (pEnt->EntFlags & Const::EntFlags::OnGround);

    constexpr int max_times = 60;
    constexpr float step_size = ((1.5f * 2.f) / max_times);
    const float latency_delay = Interfaces::EngineClient->GetNetChannelInfo()->GetLatency(Const::NetMsgFlowType::Outgoing) + cvar_cl_latency()->FloatValue;

    for (int i = 0; i < max_times; i++, current_time += step_size)
    {
        last_pos = Prediction_NextStep(last_pos, velocity, acceleration, step_size, minmax, on_ground);

        float position_time = static_cast<float>(local_eye_pos.distance_to(last_pos)) + latency_delay;
        if (float delta = std::fabsf(position_time - current_time); delta < min_delta)
        {
            min_delta = delta;
            best_time = current_time;
            best_pos = last_pos;
        }
    }

    best_time += latency_delay;
    best_pos[2] += (cvar_sv_cheats()->FloatValue / 2.f) * best_time * best_time * projectile_gravity;
    // S = at^2/2 ; t = sqrt(2S/a)
    Vector3D_F res = best_pos + origin_to_pos;
    Vector3D_F initial_res{ res[0], res[1], res[2] - proj_startvelocity * (best_time - first_time)};
    return { res, initial_res };
}



static TF2::ConVar* cvar_sv_cheats()
{
	static ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
	return sv_cheats;
}

static TF2::ConVar* cvar_cl_latency()
{
	static ConVar* sv_cheats = Interfaces::CVar->FindVar("cl_latency");
	return sv_cheats;
}


TF2_NAMESPACE_END();