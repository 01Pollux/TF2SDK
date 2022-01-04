#pragma once

#include "Math/Matrix.hpp"
#include "utils/UtlSymbol.hpp"

TF2_NAMESPACE_BEGIN();

class IMaterial;
class ITexture;

namespace Const
{
	enum class MaterialVarType
	{
		Float,
		String,
		Vector,
		Texture,
		Int,
		FourCC,
		Undefined,
		Matrix,
		Material
	};

	typedef unsigned short MaterialVarSym;
}


class IMaterialVar
{
public:
	typedef unsigned long FourCC;

protected:
	// base data and accessors
	char*		StringVal;
	int			IntVal;
	Vector4D_F	VecVal;

	// member data. total = 4 bytes
	uint8_t m_Type : 4;
	uint8_t m_nNumVectorComps : 3;
	uint8_t m_bFakeMaterialVar : 1;
	uint8_t m_nTempIndex;
	utils::UtlSymbol m_Name;

public:
	// class factory methods
	virtual ITexture* GetTextureValue() abstract;

	virtual char const* GetName() const abstract;
	virtual Const::MaterialVarSym	GetNameAsSymbol() const abstract;

	virtual void			SetFloatValue(float val) abstract;

	virtual void			SetIntValue(int val) abstract;

	virtual void			SetStringValue(char const* val) abstract;
	virtual char const* GetStringValue() const abstract;

	// Use FourCC values to pass app-defined data structures between
	// the proxy and the shader. The shader should ignore the data if
	// its FourCC type not correct.	
	virtual void			SetFourCCValue(FourCC type, void* pData) abstract;
	virtual void			GetFourCCValue(FourCC* type, void** ppData) abstract;

	// Vec (dim 2-4)
	virtual void			SetVecValue(float const* val, int numcomps) abstract;
	virtual void			SetVecValue(float x, float y) abstract;
	virtual void			SetVecValue(float x, float y, float z) abstract;
	virtual void			SetVecValue(float x, float y, float z, float w) abstract;
	virtual void			GetLinearVecValue(float* val, int numcomps) const abstract;

	// revisit: is this a good interface for textures?
	virtual void			SetTextureValue(ITexture*) abstract;

	virtual IMaterial* GetMaterialValue() abstract;
	virtual void			SetMaterialValue(IMaterial*) abstract;

	virtual bool			IsDefined() const abstract;
	virtual void			SetUndefined() abstract;

	// Matrix
	virtual void			SetMatrixValue(const Matrix4x4_F& matrix) abstract;
	virtual const Matrix4x4_F& GetMatrixValue() abstract;
	virtual bool			MatrixIsIdentity() const abstract;

	// Copy....
	virtual void			CopyFrom(IMaterialVar* pMaterialVar) abstract;

	virtual void			SetValueAutodetectType(char const* val) abstract;

	virtual IMaterial* GetOwningMaterial() abstract;

	//set just 1 component
	virtual void			SetVecComponentValue(float fVal, int nComponent) abstract;

protected:
	virtual int				GetIntValueInternal() const abstract;
	virtual float			GetFloatValueInternal() const abstract;
	virtual float const*	GetVecValueInternal() const abstract;
	virtual void			GetVecValueInternal(float* val, int numcomps) const abstract;
	virtual int				VectorSizeInternal() const abstract;

public:
	Const::MaterialVarType GetType() const
	{
		return static_cast<Const::MaterialVarType>(m_Type);
	}

	bool IsTexture() const
	{
		return GetType() == Const::MaterialVarType::Texture;
	}

	operator ITexture*()
	{
		return GetTextureValue();
	}

	// NOTE: Fast methods should only be called in thread-safe situations
	int GetIntValueFast() const
	{
		// Set methods for float and vector update this
		return IntVal;
	}

	float GetFloatValueFast() const
	{
		return VecVal[0];
	}

	float const* GetVecValueFast() const
	{
		return VecVal.data();
	}

	void GetVecValueFast(float* val, int numcomps) const
	{
		for (int i = 0; i < numcomps; i++)
		{
			val[i] = VecVal[i];
		}
	}

	int VectorSizeFast() const
	{
		return m_nNumVectorComps;
	}

	int GetIntValue() const
	{
		return GetIntValueFast();
	}

	float GetFloatValue() const
	{
		return GetFloatValueFast();
	}

	float const* GetVecValue() const
	{
		return GetVecValueFast();
	}

	void GetVecValue(float* val, int numcomps) const
	{
		GetVecValueFast(val, numcomps);
	}

	int VectorSize() const
	{
		return VectorSizeFast();
	}

private:
	void SetTempIndex(int nIndex)
	{
		m_nTempIndex = nIndex;
	}
};


TF2_NAMESPACE_END();
