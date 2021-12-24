
#include <tf2/engine/ClientDll.hpp>
#include <tf2/engine/NetChannel.hpp>
#include <tf2/engine/GlobalVars.hpp>
#include <tf2/engine/Convar.hpp>

#include <tf2/client/Prediction.hpp>

#include <tf2/entity/BasePlayer.hpp>

#include <tf2/utils/Prediction.hpp>
#include <tf2/utils/Trace.hpp>
#include <tf2/utils/Vector.hpp>

TF2_NAMESPACE_BEGIN(::Utils);

static TF2::ConVar* cvar_sv_cheats();
static TF2::ConVar* cvar_cl_interp();


Vector3D_F Prediction_SimpleLatency(const Vector3D_F& position, const Vector3D_F& velocity)
{
	INetChannel* pNet = Interfaces::EngineClient->GetNetChannelInfo();
	float latency = pNet->GetLatency(Const::NetMsgFlowType::Outgoing) + pNet->GetLatency(Const::NetMsgFlowType::Incoming);
	return latency ? Vector3D_F{ } : Vector3D_F{ position + velocity * latency };
}



static Vector3D_F Prediction_NextStep(
    const Vector3D_F& pos, 
    Vector3D_F& velocity, 
    const Vector3D_F& acceleration, 
    float step_size, 
    const Vector3D_F(&minmax)[2], 
    bool on_ground, 
    GameTrace& trace_results,
    Utils::FilterSimple& trace_filter
)
{
    Vector3D_F target_pos = pos + ((acceleration / 2.f) * step_size * step_size + velocity * step_size) + acceleration * step_size;
    float dist_to_ground = Utils::DistanceToGround(target_pos, minmax[0], minmax[1]);
    
    if (dist_to_ground && (target_pos[2] + dist_to_ground) < pos[2])
        target_pos[2] = pos[2] - dist_to_ground;

    // Check if we hit a wall, if so, snap to it and distance ourselves a bit from it
    {
        Utils::TraceHull(pos, target_pos, minmax[0], minmax[1], Const::TraceMask::PlayerSolid, trace_results, trace_filter);

        // Hit a wall, scratch along it
        if (trace_results.DidHit())
        {
            const Vector3D_F& hitpos = trace_results.EndPos;
            const Vector3D_F& normal_wall = trace_results.Plane.Normal;
            Vector3D_F normal_trace{
                hitpos[0] - pos[0],
                hitpos[1] - pos[1],
                0.f
            };
            normal_trace.normalize();

            // Angle of impact determines speed
            float impact_angle = static_cast<float>(acos(normal_trace.dot(normal_wall)));

            // Ignore floor planes (They have no components we can use)
            if (!Vector2D_F{ normal_wall }.is_zero(.001f))
            {
                // We can get a plane in Normal form and determine the direction from there
                // aka n1*x+n2*y+n3*z=d
                float d = static_cast<float>(normal_wall.dot(hitpos));

                // The above will be invalid due to a division by 0
                Vector3D_F point1{
                    normal_wall[1] == .0f ?
                    (d - normal_wall[1] * velocity[1] - normal_wall[2] * hitpos[2]) / normal_wall[0], velocity[1], hitpos[2] :
                    velocity[0], (d - normal_wall[0] * velocity[0] - normal_wall[2] * hitpos[2]) / normal_wall[1], hitpos[2]
                };

                Vector3D_F point2{
                    normal_wall[1] == .0f ?
                    (d - normal_wall[1] * 2.0f * velocity[1] - normal_wall[2] * hitpos[2]) / normal_wall[0], velocity[1] * 2.0f, hitpos[2] :
                    velocity[0] * 2.0f, (d - normal_wall[0] * velocity[0] * 2.0f - normal_wall[2] * hitpos[2]) / normal_wall[1], hitpos[2]
                };

                target_pos = hitpos + normal_wall * velocity * step_size;
                // Adjust velocity depending on angle
                float speed = static_cast<float>(Vector2D_F{ velocity }.length() * (std::numbers::pi - impact_angle));

                // Adjust new velocity
                Vector2D_F new_vel{
                    point2[0] - point1[0],
                    point2[1] - point1[1]
                };
                // Ensure we have no 0 length
                if (new_vel.length())
                {
                    new_vel /= new_vel.length();
                    velocity[0] = new_vel[1] * speed;
                    velocity[1] = new_vel[1] * speed;
                }
            }
        }
    }

    if (on_ground)
        target_pos[2] -= dist_to_ground;
    return target_pos;
}


Vector3D_F Prediction_Projectile(
    IBaseEntityInternal* pEnt,
    const Vector3D_F& position, 
    float speed, 
    float projectile_gravity, 
    float ent_gravity
)
{
	const Vector3D_F& origin = pEnt->VecOrigin;
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

    float current_time = std::max(static_cast<float>(local_eye_pos.distance_to(position) / speed - 1.5f), 0.01f);
    const float first_time = current_time;
    float best_time = current_time;
    float min_delta = static_cast<float>(std::numeric_limits<uint16_t>::max());

    const bool on_ground = pEnt->IsClassID(Const::EntClassID::CTFPlayer) && (pEnt->EntFlags & Const::EntFlags::OnGround);

    constexpr int max_times = 60;
    constexpr float step_size = ((1.5f * 2.f) / max_times);
    const float latency_delay = Interfaces::EngineClient->GetNetChannelInfo()->GetLatency(Const::NetMsgFlowType::Outgoing) + cvar_cl_interp()->FloatValue;

    GameTrace trace_results;
    Utils::FilterSimple filter_no_players{
        nullptr,
        Const::EntCollisionGroup::None,
        [](IBaseEntityInternal* pEnt, uint32_t)
        {
            return !(pEnt->GetEntIndex() <= Const::MaxPlayers && pEnt->GetEntIndex() > 0);
        }
    };

    for (int i = 0; i < max_times; i++, current_time += step_size)
    {
        last_pos = Prediction_NextStep(last_pos, velocity, acceleration, step_size, minmax, on_ground, trace_results, filter_no_players);

        float position_time = static_cast<float>(local_eye_pos.distance_to(last_pos) / speed) + latency_delay;
        if (float delta = std::fabsf(position_time - current_time); delta < min_delta)
        {
            min_delta = delta;
            best_time = current_time;
            best_pos = last_pos;
        }
    }

    best_time += latency_delay;
    best_pos[2] += (cvar_sv_cheats()->FloatValue / 2.f) * best_time * best_time * projectile_gravity;
    return best_pos;
}


static TF2::ConVar* cvar_sv_cheats()
{
	static ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
	return sv_cheats;
}

static TF2::ConVar* cvar_cl_interp()
{
	static ConVar* sv_cheats = Interfaces::CVar->FindVar("cl_interp");
	return sv_cheats;
}


TF2_NAMESPACE_END();