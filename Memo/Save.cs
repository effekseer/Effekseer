
public class Script
{
	public static string UniqueName
	{
		get
		{
			return "Save";
		}
	}	
	
	public static string Author
	{
		get
		{
			return "ShimpeiSawada";
		}
	}

	public static string Title
	{
		get
		{
			return "保存";
		}
	}

	public static string Description
	{
		get
		{
			return "ファイルを保存する。";
		}
	}

	public static void Call()
	{
		System.Console.WriteLine("Call");
	}
}
