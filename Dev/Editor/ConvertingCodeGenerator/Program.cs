using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;

namespace ConvertingCodeGenerator
{
	class Program
	{
		[STAThread]
		static void Main(string[] args)
		{
			CodeGenerator gen = new CodeGenerator();
			gen.DefaultNamespace = "Effekseer.EffectAsset";
			gen.AddTypeConverter(gen.CreateDefinedType(typeof(Effekseer.Data.LocationValues)), gen.CreateDefinedType("Effekseer.EffectAsset", "PositionParameter", null));
			gen.AddTypeConverter(gen.CreateDefinedType(typeof(Effekseer.Data.RotationValues)), gen.CreateDefinedType("Effekseer.EffectAsset", "RotationParameter", null));

			gen.AddDefinitionText(typeof(Effekseer.Data.LocationValues), "PositionParameter.cs");
			gen.AddDefinitionText(typeof(Effekseer.Data.RotationValues), "RotationParameter.cs");

			gen.Generate(new[] {
				typeof(Effekseer.Data.LocationValues),
				typeof(Effekseer.Data.RotationValues),
			},
			"ConversionsGenerated.cs",
			"Definitions.cs");
		}
	}

	public class DefinedType
	{
		public string Namespace;
		public string Name;
		public DefinedType Reflected;

		public string GlobalName
		{
			get
			{
				string ret = string.Empty;

				if (Reflected != null)
				{
					ret += Reflected.GlobalName + ".";
				}
				else
				{
					if (!string.IsNullOrEmpty(Namespace))
					{
						ret += Namespace + ".";
					}
				}

				ret += Name;

				return ret;
			}
		}
	}

	class CodeGenerator
	{
		class TargetType
		{
			public string Name;
			public Func<Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.Parameter, string> ConvertDefinition;
		}

		public class TextData
		{
			public string Path = string.Empty;
			public string Text = string.Empty;
		}

		Dictionary<Type, TargetType> targetTypes = new Dictionary<Type, TargetType>();

		Dictionary<Type, DefinedType> typeToDefinedTypes = new Dictionary<Type, DefinedType>();

		Dictionary<DefinedType, DefinedType> typeConverter = new Dictionary<DefinedType, DefinedType>();

		Dictionary<Type, TextData> typeToTextData = new Dictionary<Type, TextData>();

		string ConversionCode { get; set; } = string.Empty;

		public HashSet<Type> IgnoredTargets { get; private set; } = new HashSet<Type>();

		public string DefaultNamespace = string.Empty;

		public DefinedType CreateDefinedType(Type type)
		{
			if (type == null)
			{
				return null;
			}

			var targetType = type;

			if (type.IsSubclassOf(typeof(Effekseer.Data.Value.EnumBase)))
			{
				targetType = type.GetGenericArguments()[0];
			}

			if (typeToDefinedTypes.ContainsKey(targetType))
			{
				return typeToDefinedTypes[targetType];
			}

			var ret = new DefinedType();
			ret.Name = targetType.Name;
			ret.Namespace = targetType.Namespace;
			ret.Reflected = CreateDefinedType(targetType.ReflectedType);
			typeToDefinedTypes.Add(targetType, ret);
			return ret;
		}

		public DefinedType CreateDefinedType(string namespaceName, string name, DefinedType reflected)
		{
			var ret = new DefinedType();
			ret.Name = name;
			ret.Namespace = namespaceName;
			ret.Reflected = reflected;
			return ret;
		}

		public void AddTypeConverter(DefinedType from, DefinedType to)
		{
			typeConverter.Add(from, to);
		}

		public void AddDefinitionText(Type type, string path)
		{
			var textData = new TextData();
			textData.Path = path;
			textData.Text = string.Empty;

			typeToTextData.Add(type, textData);
		}

		public CodeGenerator()
		{
			targetTypes.Add(typeof(Effekseer.Data.Value.Boolean), new TargetType { Name = typeof(bool).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionBoolean });
			targetTypes.Add(typeof(Effekseer.Data.Value.Int), new TargetType { Name = typeof(int).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionInt });
			targetTypes.Add(typeof(Effekseer.Data.Value.Float), new TargetType { Name = typeof(float).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionFloat });
			targetTypes.Add(typeof(Effekseer.Data.Value.Vector3D), new TargetType { Name = typeof(Effekseer.Vector3F).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionVector3D });
			targetTypes.Add(typeof(Effekseer.Data.Value.FloatWithRandom), new TargetType { Name = typeof(Effekseer.EffectAsset.FloatWithRange).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionFloatWithRandom });
			targetTypes.Add(typeof(Effekseer.Data.Value.Vector3DWithRandom), new TargetType { Name = typeof(Effekseer.EffectAsset.Vector3WithRange).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionVector3DWithRandom });
			targetTypes.Add(typeof(Effekseer.Data.Value.PathForCurve), new TargetType { Name = typeof(Effekseer.EffectAsset.CurveAsset).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionPathForCurve });
			targetTypes.Add(typeof(Effekseer.Data.Value.FCurveVector3D), new TargetType { Name = typeof(Effekseer.EffectAsset.FCurveVector3D).FullName, ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionFCurveVector3D });

			{
				var textData = new TextData();
				textData.Path = string.Empty;
				textData.Text = string.Empty;

				typeToTextData.Add(typeof(Object), textData);
			}
		}

		public void Generate(Type[] types, string conversionTextPath, string definitionMiscTextPath)
		{
			ConversionCode += @"
namespace Effekseer.Compatibility.Conversion {
public partial class ConversionFormatFrom1To2 {

";

			foreach (var type in types)
			{
				Generate(type, null);
			}

			ConversionCode += @"
}
}
";
			File.WriteAllText(conversionTextPath, ConversionCode);
			File.WriteAllText(definitionMiscTextPath, typeToTextData[typeof(Object)].Text);

			foreach (var tt in typeToTextData)
			{
				if (tt.Key == typeof(Object))
				{
					continue;
				}

				File.WriteAllText(tt.Value.Path, tt.Value.Text);
			}
		}

		void Generate(Type type, object instance)
		{
			if (type.IsSubclassOf(typeof(Effekseer.Data.Value.EnumBase)))
			{
				GenerateEnum(instance as Effekseer.Data.Value.EnumBase);
			}
			else
			{
				GenerateOthers(type, instance);
			}
		}

		void GenerateEnum(Effekseer.Data.Value.EnumBase value)
		{
			var enumType = value.GetEnumType();
			var intValue = value.GetDefaultValueAsInt();
			var values = Enum.GetValues(enumType);

			object enumValue = null;

			foreach (var v in values)
			{
				if (Convert.ToInt32(v) == intValue)
				{
					enumValue = v;
					break;
				}
			}

			var (srcDefinedType, dstDefinedType) = GetSrcDstDefinedType(enumType);

			string ccode = string.Empty;
			ccode = $"public {dstDefinedType.GlobalName} ConvertValue(Effekseer.Data.Value.Enum<{srcDefinedType.GlobalName}> src) {{\n";
			ccode += $"var dst = ({dstDefinedType.GlobalName})src.Value;\n";
			ccode += "return dst;\n";
			ccode += "}\n";
			ConversionCode += ccode;

			{
				var tt = new TargetType();
				tt.Name = enumType.FullName;
				tt.ConvertDefinition = Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.ConvertDefinitionEnumBase;
				targetTypes.Add(value.GetType(), tt);
			}

			var definitionTextData = GetTextData(GetRootType(enumType));

			string dcode = string.Empty;

			var hasNamespace = !string.IsNullOrEmpty(dstDefinedType.Namespace);

			if (hasNamespace)
			{
				dcode += GetNamespaceBegin(dstDefinedType.Namespace);
			}

			var hasReflected = dstDefinedType.Reflected != null;

			if (hasReflected)
			{
				dcode += GetReflectedBegin(dstDefinedType);
			}

			dcode += $"public enum {dstDefinedType.Name} {{\n";

			var fields = enumType.GetFields();

			foreach (var field in fields)
			{
				if (field.FieldType != enumType)
				{
					continue;
				}

				dcode += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(field.GetCustomAttributes<Attribute>().ToArray());
				dcode += $"{field.Name} = {Convert.ToInt32(field.GetValue(null))},\n";
			}

			dcode += "}\n";

			if (hasReflected)
			{
				dcode += GetReflectedEnd(dstDefinedType);
			}

			if (hasNamespace)
			{
				dcode += GetNamespaceEnd();
			}

			dcode += "\n";

			definitionTextData.Text += dcode;
		}

		void GenerateOthers(Type type, object instance)
		{
			if (instance == null)
			{
				instance = Activator.CreateInstance(type, true);
			}

			var (srcDefinedType, dstDefinedType) = GetSrcDstDefinedType(type);

			{
				Func<Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.Parameter, string> func = (Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.Parameter param) =>
				{
					var str = string.Empty;
					str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(param.attributes);
					str += $"public {param.DstType.GlobalName} {param.Name} = new {param.DstType.Name}();\n";
					return str;
				};

				var tt = new TargetType();
				tt.Name = dstDefinedType.GlobalName;
				tt.ConvertDefinition = func;
				targetTypes.Add(type, tt);
			}

			var definitionTextData = GetTextData(GetRootType(type));

			string dcode = string.Empty;
			string ccode = string.Empty;

			var hasNamespace = !string.IsNullOrEmpty(dstDefinedType.Namespace);

			if (hasNamespace)
			{
				dcode += GetNamespaceBegin(dstDefinedType.Namespace);
			}

			var hasReflected = dstDefinedType.Reflected != null;

			if (hasReflected)
			{
				dcode += GetReflectedBegin(dstDefinedType);
			}

			dcode += $"public partial class {dstDefinedType.Name} {{\n";

			ccode = $"public {dstDefinedType.GlobalName} ConvertValue({srcDefinedType.GlobalName} src) {{\n";
			ccode += $"var dst = new {dstDefinedType.GlobalName}();\n";

			var fieldDefinitionCodes = new List<string>();

			foreach (var prop in type.GetProperties())
			{
				var propType = prop.PropertyType;

				if (IgnoredTargets.Contains(propType))
				{
					continue;
				}

				if (!targetTypes.ContainsKey(propType))
				{
					var value = prop.GetValue(instance);
					Generate(propType, value);
				}

				var (srcPropDefinedType, dstPropDefinedType) = GetSrcDstDefinedType(prop.PropertyType);

				var param = new Effekseer.Compatibility.Conversion.DefinitionGeneratorFrom1To2.Parameter();
				param.Name = prop.Name;
				param.attributes = prop.GetCustomAttributes().ToArray();
				param.Value = prop.GetValue(instance);
				param.SrcType = srcPropDefinedType;
				param.DstType = dstPropDefinedType;

				var info = targetTypes[propType];

				fieldDefinitionCodes.Add(info.ConvertDefinition?.Invoke(param));
				ccode += $"dst.{prop.Name} = ConvertValue(src.{prop.Name});\n";
			}

			dcode += string.Join("\n\n", fieldDefinitionCodes);
			dcode += "\n";

			ccode += "return dst;\n";

			dcode += "}\n";

			if (hasReflected)
			{
				dcode += GetReflectedEnd(dstDefinedType);
			}

			if (hasNamespace)
			{
				dcode += GetNamespaceEnd();
			}

			dcode += "\n";

			ccode += "}\n";

			definitionTextData.Text += dcode;
			ConversionCode += ccode;
		}

		Tuple<DefinedType, DefinedType> GetSrcDstDefinedType(Type type)
		{
			var srcDefinedType = CreateDefinedType(type);

			DefinedType dstDefinedType = null;
			if (typeConverter.ContainsKey(srcDefinedType))
			{
				dstDefinedType = typeConverter[srcDefinedType];
			}
			else
			{
				dstDefinedType = CreateDefinedType(DefaultNamespace, type.Name, null);
				AddTypeConverter(srcDefinedType, dstDefinedType);

				if (type.ReflectedType != null)
				{
					dstDefinedType.Reflected = GetSrcDstDefinedType(type.ReflectedType).Item2;
				}
			}

			return Tuple.Create(srcDefinedType, dstDefinedType);
		}

		string GetReflectedBegin(DefinedType type)
		{
			string Export(DefinedType type, string ret)
			{
				if (type.Reflected == null)
				{
					return ret;
				}

				ret = "public partial class " + type.Reflected.Name + " {\n" + ret;
				return Export(type.Reflected, ret);
			}

			return Export(type, string.Empty);
		}

		string GetReflectedEnd(DefinedType type)
		{
			string Export(DefinedType type, string ret)
			{
				if (type.Reflected == null)
				{
					return ret;
				}

				ret = "}\n" + ret;
				return Export(type.Reflected, ret);
			}

			return Export(type, string.Empty);
		}

		string GetNamespaceBegin(string ns)
		{
			return $"namespace {ns} {{\n";
		}

		string GetNamespaceEnd()
		{
			return "}\n";
		}

		TextData GetTextData(Type type)
		{
			if (typeToTextData.TryGetValue(type, out var text))
			{
				return text;
			}

			return typeToTextData[typeof(Object)];
		}

		Type GetRootType(Type type)
		{
			if (type.ReflectedType != null)
			{
				return GetRootType(type.ReflectedType);
			}

			return type;
		}
	}

}

namespace Effekseer.Compatibility.Conversion
{
	class ConversionUtils
	{
		public static string TryAddAttribute(Attribute[] attributes)
		{
			string ret = string.Empty;

			{
				var att = attributes.OfType<Effekseer.KeyAttribute>().FirstOrDefault();
				if (att != null)
				{
					ret += $"[Key(key = \"{att.key}\")]\n";
				}
			}

			{
				var att = attributes.OfType<Effekseer.Data.SelectorAttribute>().FirstOrDefault();
				if (att != null)
				{
					ret += $"[VisiblityController(ID = {att.ID})]\n";
				}
			}

			{
				var att = attributes.OfType<Effekseer.Data.SelectedAttribute>().FirstOrDefault();
				if (att != null)
				{
					ret += $"[VisiblityControlled(ID = {att.ID}, Value = {att.Value})]\n";
				}
			}

			return ret;
		}
	}

	partial class DefinitionGeneratorFrom1To2
	{
		public class Parameter
		{
			public string Name;
			public Attribute[] attributes;
			public object Value;
			public ConvertingCodeGenerator.DefinedType DstType;
			public ConvertingCodeGenerator.DefinedType SrcType;
		}

		public static string ConvertDefinitionDynamicEquation(Parameter parameter)
		{
			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public EffectAsset.DynamicEquation {parameter.Name} = null;";
			return str;
		}

		public static string ConvertDefinitionBoolean(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.Boolean;
			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			if (value.DefaultValue)
			{
				str += $"public bool {parameter.Name} = true;";
			}
			else
			{
				str += $"public bool {parameter.Name} = false;";
			}

			return str;
		}

		public static string ConvertDefinitionInt(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.Int;
			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public int {parameter.Name} = {value.DefaultValue};";
			return str;
		}

		public static string ConvertDefinitionFloat(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.Float;
			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public float {parameter.Name} = {value.DefaultValue};";
			return str;
		}

		public static string ConvertDefinitionVector3D(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.Vector3D;
			var typename = typeof(Effekseer.EffectAsset.Vector3).FullName;

			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public {typename} {parameter.Name} = new {typename}();";
			return str;
		}

		public static string ConvertDefinitionFloatWithRandom(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.FloatWithRandom;
			var typename = typeof(Effekseer.EffectAsset.FloatWithRange).FullName;

			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public {typename} {parameter.Name} = new {typename}();";
			return str;
		}

		public static string ConvertDefinitionVector3DWithRandom(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.Vector3DWithRandom;
			var typename = typeof(Effekseer.EffectAsset.Vector3WithRange).FullName;

			var min = value.X.ValueMin;
			var max = value.X.ValueMax;

			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"[Effekseer.EffectAsset.FloatRange(Min = {GetStr(min)}, Max = {GetStr(max)})]\n";
			str += $"public {typename} {parameter.Name} = new {typename}();";
			return str;
		}

		public static string ConvertDefinitionEnumBase(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.EnumBase;
			var enumType = value.GetEnumType();
			var intValue = value.GetDefaultValueAsInt();
			var values = Enum.GetValues(enumType);

			object enumValue = null;

			foreach (var v in values)
			{
				if (Convert.ToInt32(v) == intValue)
				{
					enumValue = v;
					break;
				}
			}

			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public {parameter.DstType.GlobalName} {parameter.Name} = {parameter.DstType.GlobalName}.{enumValue};";
			return str;
		}

		public static string ConvertDefinitionPathForCurve(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.PathForCurve;
			var typename = typeof(Effekseer.EffectAsset.CurveAsset).FullName;

			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public {typename} {parameter.Name} = null;";
			return str;
		}

		public static string ConvertDefinitionFCurveVector3D(Parameter parameter)
		{
			var value = parameter.Value as Effekseer.Data.Value.FCurveVector3D;
			var typename = typeof(Effekseer.EffectAsset.FCurveVector3D).FullName;

			var str = string.Empty;
			str += Effekseer.Compatibility.Conversion.ConversionUtils.TryAddAttribute(parameter.attributes);
			str += $"public {typename} {parameter.Name} = new {typename}();";
			return str;
		}

		static string GetStr(float value)
		{
			if (value == float.MaxValue)
			{
				return "float.MaxValue";
			}
			else if (value == float.MinValue)
			{
				return "float.MinValue";
			}

			return value.ToString();
		}
	}
}
