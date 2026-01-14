#include "EfkRes.GEOLoader.h"
#include "EfkRes.Utils.h"
#include <fstream>
#include <json.hpp>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace efkres
{

namespace
{
using namespace nlohmann;

class BgeoParser
{
	static inline constexpr uint8_t JID_MAGIC = 0x7f;
	static inline constexpr uint32_t JID_BINARY_MAGIC = 0x624a534e;

	static inline constexpr uint8_t JID_NULL = 0x00;
	static inline constexpr uint8_t JID_MAP_BEGIN = 0x7b;
	static inline constexpr uint8_t JID_MAP_END = 0x7d;
	static inline constexpr uint8_t JID_ARRAY_BEGIN = 0x5b;
	static inline constexpr uint8_t JID_ARRAY_END = 0x5d;
	static inline constexpr uint8_t JID_BOOL = 0x10;
	static inline constexpr uint8_t JID_INT8 = 0x11;
	static inline constexpr uint8_t JID_INT16 = 0x12;
	static inline constexpr uint8_t JID_INT32 = 0x13;
	static inline constexpr uint8_t JID_INT64 = 0x14;
	static inline constexpr uint8_t JID_UINT8 = 0x21;
	static inline constexpr uint8_t JID_UINT16 = 0x22;
	static inline constexpr uint8_t JID_REAL16 = 0x18;
	static inline constexpr uint8_t JID_REAL32 = 0x19;
	static inline constexpr uint8_t JID_REAL64 = 0x1a;
	static inline constexpr uint8_t JID_STRING = 0x27;
	static inline constexpr uint8_t JID_FALSE = 0x30;
	static inline constexpr uint8_t JID_TRUE = 0x31;
	static inline constexpr uint8_t JID_TOKENDEF = 0x2b;
	static inline constexpr uint8_t JID_TOKENREF = 0x26;
	static inline constexpr uint8_t JID_TOKENUNDEF = 0x2d;
	static inline constexpr uint8_t JID_UNIFORM_ARRAY = 0x40;
	static inline constexpr uint8_t JID_KEY_SEPARATOR = 0x3a;
	static inline constexpr uint8_t JID_VALUE_SEPARATOR = 0x2c;

private:
	BinaryReader m_reader;
	std::array<json, 256> m_stringTable;

public:
	BgeoParser() = default;

	json Parse(const std::string& filename)
	{
		if (!m_reader.Open(filename.c_str()))
		{
			return false;
		}

		if (m_reader.Read<uint8_t>() != JID_MAGIC)
		{
			return false;
		}

		if (m_reader.Read<uint32_t>() != JID_BINARY_MAGIC)
		{
			return false;
		}

		return ParseValue();
	}

private:
	json ParseValue(uint8_t type = 0xFF)
	{
		if (type == 0xFF)
		{
			type = m_reader.Read<uint8_t>();
		}

		switch (type)
		{
		case JID_NULL:
			return nullptr;
		case JID_MAP_BEGIN:
			return ParseDict();
		case JID_ARRAY_BEGIN:
			return ParseList();
		case JID_BOOL:
			return m_reader.Read<uint8_t>() == 0 ? false : true;
		case JID_INT8:
			return m_reader.Read<int8_t>();
		case JID_INT16:
			return m_reader.Read<int16_t>();
		case JID_INT32:
			return m_reader.Read<int32_t>();
		case JID_INT64:
			return m_reader.Read<int64_t>();
		case JID_UINT8:
			return m_reader.Read<uint8_t>();
		case JID_UINT16:
			return m_reader.Read<uint16_t>();
		// case JID_REAL16:
		//	return m_reader.Read<float16_t>();
		case JID_REAL32:
			return m_reader.Read<float>();
		case JID_REAL64:
			return m_reader.Read<double>();
		case JID_STRING:
			return ParseString();
		case JID_TOKENDEF:
			ParseTokenDef();
			return ParseValue();
		case JID_TOKENREF:
			return ParseTokenRef();
		case JID_TOKENUNDEF:
			ParseTokenUndef();
			return ParseValue();
		case JID_UNIFORM_ARRAY:
			return ParseUniformArray();
		default:
			break;
		}
		return nullptr;
	}

	size_t ParseSize()
	{
		uint8_t size = m_reader.Read<uint8_t>();
		if (size <= 0xf1)
		{
			return size;
		}
		else if (size == 0xf2)
		{
			return m_reader.Read<uint16_t>();
		}
		else if (size == 0xf4)
		{
			return m_reader.Read<uint32_t>();
		}
		else if (size == 0xf8)
		{
			return m_reader.Read<uint64_t>();
		}
		return 0;
	}

	json ParseDict()
	{
		json dict = json::object();
		while (true)
		{
			uint8_t type = m_reader.Read<uint8_t>();
			if (type == JID_MAP_END)
			{
				break;
			}
			else if (type == JID_KEY_SEPARATOR)
			{
				continue;
			}
			else if (type == JID_VALUE_SEPARATOR)
			{
				continue;
			}

			json key = ParseValue(type);
			json value = ParseValue();
			dict[key.get<std::string>()] = value;
		}
		return dict;
	}

	json ParseList()
	{
		json list = json::array();
		while (true)
		{
			uint8_t type = m_reader.Read<uint8_t>();
			if (type == JID_ARRAY_END)
			{
				break;
			}
			else if (type == JID_KEY_SEPARATOR)
			{
				continue;
			}
			else if (type == JID_VALUE_SEPARATOR)
			{
				continue;
			}

			json value = ParseValue(type);
			if (value == "primitives")
			{
				value = value;
			}
			list.push_back(value);
		}
		return list;
	}

	json ParseString()
	{
		size_t size = ParseSize();
		std::string str;
		str.resize(size);
		m_reader.Read(str.data(), size);
		return str;
	}

	void ParseTokenDef()
	{
		uint8_t id = m_reader.Read<uint8_t>();
		json value = ParseString();
		m_stringTable[id] = value;
	}

	json ParseTokenRef()
	{
		uint8_t id = m_reader.Read<uint8_t>();
		return m_stringTable[id];
	}

	void ParseTokenUndef()
	{
		uint8_t id = m_reader.Read<uint8_t>();
		m_stringTable[id] = nullptr;
	}

	json ParseUniformArray()
	{
		uint8_t type = m_reader.Read<uint8_t>();
		size_t size = ParseSize();
		json list = json::array();
		for (size_t i = 0; i < size; i++)
		{
			list.push_back(ParseValue(type));
		}
		return list;
	}
};

static json FindOrNull(const json& dict, const char* key)
{
	auto it = dict.find(key);
	return (it != dict.end()) ? it.value() : json(nullptr);
}

class GeoAttribute
{
public:
	std::string name;

	using Values = std::variant<std::vector<float>, std::vector<Vec2>, std::vector<Vec3>, std::vector<Vec4>>;
	Values values;

public:
	template <class T> bool HasValueType() { return std::holds_alternative<std::vector<T>>(values); }
	template <class T> T GetValue(size_t index) { return std::get<std::vector<T>>(values)[index]; }
};

class GeoAttributeList
{
public:
	std::vector<GeoAttribute> attributes;

public:
	GeoAttribute* Get(const std::string& attr)
	{
		for (auto& attribute : attributes)
		{
			if (attribute.name == attr)
			{
				return &attribute;
			}
		}
		return nullptr;
	}

	void ParseJson(const json& data)
	{
		for (auto& attr : data)
		{
			if (attr["type"] == "numeric")
			{
				GeoAttribute& attribute = attributes.emplace_back();
				attribute.name = attr["name"];

				int32_t size = attr["size"];
				json values = attr["values"];

				json arrays = FindOrNull(values, "arrays");
				json tuples = FindOrNull(values, "tuples");
				json rawpagedata = FindOrNull(values, "rawpagedata");

				if (size == 1)
				{
					std::vector<float> values;
					for (const json& v : arrays[0])
					{
						values.push_back(v);
					}
					attribute.values = std::move(values);
				}
				else if (size == 2)
				{
					std::vector<Vec2> values;
					if (tuples.is_array())
					{
						for (const json& v : tuples)
						{
							values.push_back(Vec2{v[0], v[1]});
						}
					}
					else if (rawpagedata.is_array())
					{
						for (size_t i = 0; i < rawpagedata.size(); i += 2)
						{
							values.push_back(Vec2{rawpagedata[i], rawpagedata[i + 1]});
						}
					}
					attribute.values = std::move(values);
				}
				else if (size == 3)
				{
					std::vector<Vec3> values;
					if (tuples.is_array())
					{
						for (const json& v : tuples)
						{
							values.push_back(Vec3{v[0], v[1], v[2]});
						}
					}
					else if (rawpagedata.is_array())
					{
						for (size_t i = 0; i < rawpagedata.size(); i += 3)
						{
							values.push_back(Vec3{rawpagedata[i], rawpagedata[i + 1], rawpagedata[i + 2]});
						}
					}
					attribute.values = std::move(values);
				}
				else if (size == 4)
				{
					std::vector<Vec4> values;
					if (tuples.is_array())
					{
						for (const json& v : tuples)
						{
							values.push_back(Vec4{v[0], v[1], v[2], v[3]});
						}
					}
					else if (rawpagedata.is_array())
					{
						for (size_t i = 0; i < rawpagedata.size(); i += 4)
						{
							values.push_back(Vec4{rawpagedata[i], rawpagedata[i + 1], rawpagedata[i + 2], rawpagedata[i + 3]});
						}
					}
					attribute.values = std::move(values);
				}
			}
		}
	}
};

class GeoDocument
{
public:
	GeoAttributeList pointAttrs;
	GeoAttributeList vertAttrs;
	GeoAttributeList primAttrs;

	std::vector<int32_t> vertPointRef;
	std::vector<std::tuple<int32_t, int32_t>> primVertRange;

public:
	void ParseJson(const json& geoData)
	{
		auto data = ParseGeoDict(geoData);
		auto& attrData = data["attributes"];
		if (auto it = attrData.find("pointattributes"); it != attrData.end())
		{
			pointAttrs.ParseJson(ParseGeoAttrs(*it));
		}

		for (int32_t index : data["topology"]["pointref"]["indices"])
		{
			vertPointRef.push_back(index);
		}

		if (auto it = attrData.find("vertexattributes"); it != attrData.end())
		{
			vertAttrs.ParseJson(ParseGeoAttrs(*it));
		}
		if (auto it = attrData.find("primitiveattributes"); it != attrData.end())
		{
			primAttrs.ParseJson(ParseGeoAttrs(*it));
		}

		auto& primData = data["primitives"][0][1];
		json startvertex = FindOrNull(primData, "startvertex");
		if (startvertex.is_null())
			startvertex = FindOrNull(primData, "s_v");
		json nverticesRules = FindOrNull(primData, "nvertices_rle");
		if (nverticesRules.is_null())
			nverticesRules = FindOrNull(primData, "r_v");

		int32_t offset = startvertex;
		for (size_t nvIndex = 0; nvIndex < nverticesRules.size(); nvIndex += 2)
		{
			int32_t nvertices = nverticesRules[nvIndex];
			int32_t nvcount = nverticesRules[nvIndex + 1];
			for (int32_t primIndex = 0; primIndex < nvcount; primIndex++)
			{
				primVertRange.emplace_back(offset, nvertices);
				offset += nvertices;
			}
		}
	}

private:
	std::map<std::string, json> ParseGeoDict(const json& data)
	{
		std::map<std::string, json> result;
		for (size_t index = 0; index < data.size(); index += 2)
		{
			auto& key = data[index];
			auto& value = data[index + 1];
			if (value.is_array())
			{
				if (value[0].is_string())
				{
					result[key] = ParseGeoDict(value);
					continue;
				}
				else if (value[0].is_array())
				{
					result[key] = ParseGeoList(value);
					continue;
				}
			}
			result[key] = value;
		}
		return result;
	}

	std::vector<json> ParseGeoList(const json& data)
	{
		std::vector<json> result;
		for (size_t index = 0; index < data.size(); index++)
		{
			auto& value = data[index];
			if (value.is_array())
			{
				if (value[0].is_string())
				{
					result.push_back(ParseGeoDict(value));
					continue;
				}
				else if (value[0].is_array())
				{
					result.push_back(ParseGeoList(value));
					continue;
				}
			}
			result.push_back(value);
		}
		return result;
	}

	json ParseGeoAttrs(const json& data)
	{
		if (data.is_null())
		{
			return {};
		}
		std::vector<json> result;
		for (auto& attrData : data)
		{
			json merged;
			for (auto& attr : attrData[0].items())
			{
				merged[attr.key()] = attr.value();
			}
			for (auto& attr : attrData[1].items())
			{
				merged[attr.key()] = attr.value();
			}
			result.push_back(merged);
		}
		return result;
	}
};

} // namespace

std::optional<Model> GEOLoader::LoadModel(std::string_view filepath)
{
	std::string_view fileExt = filepath.substr(filepath.find_last_of('.'));

	json jsonData;

	if (fileExt == ".bgeo")
	{
		BgeoParser parser;
		jsonData = parser.Parse(std::string(filepath));
	}
	else if (fileExt == ".geo")
	{
		jsonData = json::parse(std::ifstream(std::string(filepath)));
	}
	else
	{
		return std::nullopt;
	}

	if (jsonData.is_null())
	{
		return std::nullopt;
	}

	GeoDocument doc;
	doc.ParseJson(jsonData);

	std::vector<std::string> vertexFormat = {"position"};
	auto pointAttr = doc.pointAttrs.Get("P");
	if (!pointAttr)
	{
		return std::nullopt;
	}

	auto normalAttr = doc.vertAttrs.Get("N");
	if (normalAttr)
	{
		vertexFormat.push_back("normal");
	}

	auto vertColorAttr = doc.vertAttrs.Get("Cd");
	auto pointColorAttr = doc.pointAttrs.Get("Cd");
	auto primColorAttr = doc.primAttrs.Get("Cd");
	auto vertAlphaAttr = doc.vertAttrs.Get("Alpha");
	auto pointAlphaAttr = doc.pointAttrs.Get("Alpha");
	auto primAlphaAttr = doc.primAttrs.Get("Alpha");
	if (vertColorAttr || pointColorAttr || primColorAttr)
	{
		vertexFormat.push_back("color");
	}

	auto uvAttr = doc.vertAttrs.Get("uv");
	auto uv1Attr = (uvAttr == nullptr) ? doc.vertAttrs.Get("uv1") : uvAttr;
	if (uv1Attr)
	{
		vertexFormat.push_back("uv1");
	}
	auto uv2Attr = doc.vertAttrs.Get("uv2");
	if (uv2Attr)
	{
		vertexFormat.push_back("uv2");
	}

	Model model;
	auto& mesh = model.meshes.emplace_back();

	for (int32_t primIndex = 0; primIndex < doc.primVertRange.size(); primIndex++)
	{
		auto& [vertOffset, vertCount] = doc.primVertRange[primIndex];
		for (int32_t vertIndex = vertOffset; vertIndex < vertOffset + vertCount; vertIndex++)
		{
			int32_t pointIndex = doc.vertPointRef[vertIndex];
			MeshVertex vertex = {};

			vertex.position = pointAttr->GetValue<Vec3>(pointIndex);

			if (normalAttr)
			{
				vertex.normal = normalAttr->GetValue<Vec3>(vertIndex);
			}

			if (vertColorAttr)
			{
				Vec3 color = vertColorAttr->GetValue<Vec3>(vertIndex);
				vertex.color = {color.x, color.y, color.z, 1.0f};
				if (vertAlphaAttr)
				{
					vertex.color.w = vertAlphaAttr->GetValue<float>(vertIndex);
				}
			}
			else if (pointColorAttr)
			{
				Vec3 color = pointColorAttr->GetValue<Vec3>(pointIndex);
				vertex.color = {color.x, color.y, color.z, 1.0f};
				if (pointAlphaAttr)
				{
					vertex.color.w = pointAlphaAttr->GetValue<float>(pointIndex);
				}
			}
			else if (primColorAttr)
			{
				const Vec3& color = primColorAttr->GetValue<Vec3>(primIndex);
				vertex.color = {color.x, color.y, color.z, 1.0f};
				if (primAlphaAttr)
				{
					vertex.color.w = primAlphaAttr->GetValue<float>(primIndex);
				}
			}
			else
			{
				vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
			}

			if (uv1Attr)
			{
				if (uv1Attr->HasValueType<Vec2>())
				{
					vertex.uv1 = uv1Attr->GetValue<Vec2>(vertIndex);
				}
				else if (uv1Attr->HasValueType<Vec3>())
				{
					auto v = uv1Attr->GetValue<Vec3>(vertIndex);
					vertex.uv1 = Vec2{v.x, v.y};
				}
			}
			if (uv2Attr)
			{
				if (uv2Attr->HasValueType<Vec2>())
				{
					vertex.uv2 = uv2Attr->GetValue<Vec2>(vertIndex);
				}
				else if (uv2Attr->HasValueType<Vec3>())
				{
					auto v = uv2Attr->GetValue<Vec3>(vertIndex);
					vertex.uv2 = Vec2{v.x, v.y};
				}
			}

			mesh.vertices.push_back(vertex);
		}

		if (vertCount == 3)
		{
			mesh.faces.push_back(MeshFace{vertOffset, vertOffset + 1, vertOffset + 2});
		}
		else if (vertCount == 4)
		{
			mesh.faces.push_back(MeshFace{vertOffset, vertOffset + 1, vertOffset + 2});
			mesh.faces.push_back(MeshFace{vertOffset, vertOffset + 2, vertOffset + 3});
		}
	}

	model.nodes.push_back(Node{&mesh, Mat43::Identity()});

	// print(f"vertex format: {vertex_format}")
	// print(f"vertex count: {len(vertices)}")
	// print(f"triangle count: {len(triangles)}");

	return std::move(model);
}

} // namespace efkres
