#include "Effekseer.InternalScript.h"
#include <assert.h>

namespace Effekseer
{

bool InternalScript::IsValidOperator(int value) const
{
	if (0 <= value && value <= 4)
		return true;
	if (11 <= value && value <= 12)
		return true;

	return false;
}

bool InternalScript::IsValidRegister(int index) const
{
	if (index < 0)
		return false;

	if (static_cast<uint32_t>(index) < registers.size())
		return true;

	if (0x1000 + 0 <= index && index <= 0x1000 + 3)
		return true;

	if (0x1000 + 0x100 + 0 <= index && index <= 0x1000 + 0x100 + 0)
		return true;

	if (0x1000 + 0x200 + 0 <= index && index <= 0x1000 + 0x200 + 4)
		return true;

	return false;
}

float InternalScript::GetRegisterValue(int index,
									   const std::array<float, 4>& externals,
									   const std::array<float, 1>& globals,
									   const std::array<float, 5>& locals) const
{
	auto ind = static_cast<uint32_t>(index);
	if (ind < registers.size())
	{
		return registers[ind];
	}
	else if (0x1000 + 0 <= ind && ind <= 0x1000 + 3)
	{
		return externals[ind - 0x1000];
	}
	else if (0x1000 + 0x100 + 0 <= ind && ind <= 0x1000 + 0x100 + 0)
	{
		return globals[ind - 0x1000 - 0x100];
	}
	else if (0x1000 + 0x200 + 0 <= ind && ind <= 0x1000 + 0x200 + 4)
	{
		return locals[ind - 0x1000 - 0x200];
	}

	assert(false);
	return 0.0f;
}

InternalScript::InternalScript() {}

InternalScript ::~InternalScript() {}
bool InternalScript::Load(uint8_t* data, int size)
{
	if (size < 20)
		return false;

	int32_t registerCount = 0;

	memcpy(&version_, data + sizeof(int) * 0, sizeof(int));
	memcpy(&runningPhase, data + sizeof(int) * 1, sizeof(int));
	memcpy(&registerCount, data + sizeof(int) * 2, sizeof(int));
	memcpy(&operatorCount_, data + sizeof(int) * 3, sizeof(int));
	memcpy(&outputRegister_, data + sizeof(int) * 4, sizeof(int));

	if (registerCount < 0)
		return false;

	registers.resize(registerCount);

	if (!IsValidRegister(outputRegister_))
	{
		return false;
	}

	operators.resize(size - 20);
	memcpy(operators.data(), data + sizeof(int) * 5, size - 20);

	// check operators
	uint32_t offset = 0;
	for (int i = 0; i < operatorCount_; i++)
	{
		// type
		OperatorType type;
		if (offset + 4 > operators.size())
			return false;

		memcpy(&type, operators.data() + offset, sizeof(OperatorType));
		offset += sizeof(int);

		if (!IsValidOperator((int)type))
			return false;

		// counter
		if (offset + 4 > operators.size())
			return false;

		int32_t inputCount = 0;
		memcpy(&inputCount, operators.data() + offset, sizeof(int));
		offset += sizeof(int);

		if (offset + 4 > operators.size())
			return false;

		int32_t outputCount = 0;
		memcpy(&outputCount, operators.data() + offset, sizeof(int));
		offset += sizeof(int);

		if (offset + 4 > operators.size())
			return false;

		int32_t attributeCount = 0;
		memcpy(&attributeCount, operators.data() + offset, sizeof(int));
		offset += sizeof(int);

		// input
		for (int j = 0; j < inputCount; j++)
		{
			if (offset + 4 > operators.size())
				return false;
			int index = 0;
			memcpy(&index, operators.data() + offset, sizeof(int));
			offset += sizeof(int);

			if (!IsValidRegister(index))
			{
				return false;
			}
		}

		// output
		for (int j = 0; j < outputCount; j++)
		{
			if (offset + 4 > operators.size())
				return false;
			int index = 0;
			memcpy(&index, operators.data() + offset, sizeof(int));
			offset += sizeof(int);

			if ((index < 0 || index >= registers.size()))
			{
				return false;
			}
		}

		// attribute
		for (int j = 0; j < attributeCount; j++)
		{
			if (offset + 4 > operators.size())
				return false;
			int index = 0;
			memcpy(&index, operators.data() + offset, sizeof(int));
			offset += sizeof(int);
		}
	}

	if (offset != operators.size())
		return false;

	isValid_ = true;

	return true;
}

float InternalScript::Execute(const std::array<float, 4>& externals,
							  const std::array<float, 1>& globals,
							  const std::array<float, 5>& locals)
{
	if (!isValid_)
		return 0.0f;

	int offset = 0;
	for (int i = 0; i < operatorCount_; i++)
	{
		// type
		OperatorType type;
		memcpy(&type, operators.data() + offset, sizeof(OperatorType));
		offset += sizeof(int);

		int32_t inputCount = 0;
		memcpy(&inputCount, operators.data() + offset, sizeof(int));
		offset += sizeof(int);

		int32_t outputCount = 0;
		memcpy(&outputCount, operators.data() + offset, sizeof(int));
		offset += sizeof(int);

		int32_t attributeCount = 0;
		memcpy(&attributeCount, operators.data() + offset, sizeof(int));
		offset += sizeof(int);

		auto inputOffset = offset;
		auto outputOffset = inputOffset + inputCount * sizeof(int);
		auto attributeOffset = outputOffset + outputCount * sizeof(int);
		offset = attributeOffset + attributeCount * sizeof(int);

		std::array<float, 4> tempInputs;

		for (int j = 0; j < inputCount; j++)
		{
			int index = 0;
			memcpy(&index, operators.data() + inputOffset, sizeof(int));
			inputOffset += sizeof(int);

			tempInputs[j] = GetRegisterValue(index, externals, globals, locals);
		}

		for (int j = 0; j < outputCount; j++)
		{
			int index = 0;
			memcpy(&index, operators.data() + outputOffset, sizeof(int));
			outputOffset += sizeof(int);

			if (type == OperatorType::Add)
				registers[index] = tempInputs[0] + tempInputs[1];
			else if (type == OperatorType::Sub)
				registers[index] = tempInputs[0] - tempInputs[1];
			else if (type == OperatorType::Mul)
				registers[index] = tempInputs[0] * tempInputs[1];
			else if (type == OperatorType::Div)
				registers[index] = tempInputs[0] / tempInputs[1];
			else if (type == OperatorType::UnaryAdd)
				registers[index] = tempInputs[0];
			else if (type == OperatorType::UnarySub)
				registers[index] = -tempInputs[0];
			else if (type == OperatorType::Constant)
			{
				float att = 0;
				memcpy(&att, operators.data() + attributeOffset, sizeof(int));
				registers[index] = att;
			}
		}
	}

	return GetRegisterValue(outputRegister_, externals, globals, locals);
}

} // namespace Effekseer