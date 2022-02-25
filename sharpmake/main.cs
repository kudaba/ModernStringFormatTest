using Sharpmake;
using System;
using System.IO;

namespace ModernStringFormat
{
    [Fragment, Flags]
    public enum Toolset
    {
		None = 1,
        Gcc = 2,
        Clang = 4,
        GCov = 8
    }

	[Fragment, Flags]
	public enum MSF_Options
	{
		None = 1,
		Validation = 2,
	}

	public class CrossTarget : Target
    {
        public CrossTarget()
        {

        }
        public CrossTarget(Platform platform, DevEnv devEnv, Optimization optim, Toolset tools = Toolset.None, MSF_Options options = MSF_Options.None | MSF_Options.Validation)
            : base(platform, devEnv, optim)
        {
            Toolset = tools;
			Options = options;
		}

		public override string Name
		{
			get
			{
				string name = base.Name;

				if (Options != MSF_Options.None)
					name += "_" + Options.ToString();

				return name;
			}
		}

		public string BuildName
		{
			get
			{
				string name = DevEnv.ToString();

				name += "_" + Platform.ToString();

				if (Toolset != Toolset.None)
					name += "_" + Toolset.ToString();

				name += "_" + Optimization.ToString();

				if (Options != MSF_Options.None)
					name += "_" + Options.ToString();

				return name;
			}
		}

		public Toolset Toolset;
        public MSF_Options Options;
	}

    public static class Globals
	{
		public const string PathToRoot = "/..";

		public const string PathToSrc = PathToRoot + "/src";
		public const string PathToSimpleTest = PathToRoot + "/simpletest";
		public const string PathToModernStringFormat = PathToRoot + "/ModernStringFormat";

		public const string PathToTmp = PathToRoot + @"/.temp";
		public const string PathToBuild = PathToTmp + @"/build";
		public const string PathToBin = PathToTmp + @"/bin";
		public const string PathToGen = PathToRoot + @"/projects";

		public static readonly CrossTarget[] DefaultTargets = new CrossTarget[]
		{
			new CrossTarget(Platform.win64 | Platform.win32,
				DevEnv.vs2019,
				Optimization.Debug | Optimization.Release,
				options: MSF_Options.None),
			new CrossTarget(Platform.win64 | Platform.win32,
				DevEnv.vs2022,
				Optimization.Debug | Optimization.Release),
			new CrossTarget(Platform.linux,
				DevEnv.make,
				Optimization.Debug | Optimization.Release,
                Toolset.Gcc | Toolset.Clang | Toolset.GCov),
            new CrossTarget(Platform.mac,
                DevEnv.make,
                Optimization.Debug | Optimization.Release,
				Toolset.Gcc | Toolset.Clang)
        };
	}

	[Generate]
	public class ModernStringFormatProject : Project
	{
		public ModernStringFormatProject() : base(typeof(CrossTarget))
		{
			AddTargets(Globals.DefaultTargets);
			SourceRootPath = "[project.SharpmakeCsPath]" + Globals.PathToSrc;
			AdditionalSourceRootPaths.Add("[project.SharpmakeCsPath]" + Globals.PathToSimpleTest);
			AdditionalSourceRootPaths.Add("[project.SharpmakeCsPath]" + Globals.PathToModernStringFormat);
			AdditionalSourceRootPaths.Add("[project.SharpmakeCsPath]" + Globals.PathToRoot);
			NoneExtensions.Add(".md");
			NoneExtensions.Add("LICENSE");
		}

		[Configure]
		public void ConfigureAll(Configuration conf, CrossTarget target)
		{
			conf.ProjectPath = "[project.SharpmakeCsPath]" + Globals.PathToGen;
			conf.TargetPath = "[project.SharpmakeCsPath]" + Globals.PathToBin;

			conf.TargetFileName = @"[project.Name]_[target.BuildName]";
			conf.IntermediatePath = "[project.SharpmakeCsPath]" + Globals.PathToBuild + @"\[target.BuildName]";

			conf.IncludePaths.Add("[project.SharpmakeCsPath]" + Globals.PathToSimpleTest);
			conf.IncludePaths.Add("[project.SharpmakeCsPath]" + Globals.PathToModernStringFormat);

			conf.Defines.Add("MSF_HEAVY_ASSERTS_ENABLED");

			if (target.Platform != Platform.win32) // have at least one set of tests that do not use it
				conf.Defines.Add("MSF_ERROR_PEDANTIC");

			if (target.Options == MSF_Options.Validation)
				conf.Defines.Add("MSF_VALIDATION_ENABLED");
		}

		[Configure(DevEnv.VisualStudio)]
		public void ConfigureVS(Configuration conf, CrossTarget target)
		{
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]";

			conf.Options.Add(Options.Vc.Compiler.Exceptions.Disable);
			conf.Options.Add(Options.Vc.General.WarningLevel.Level4);
			conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);

			conf.Defines.Add("_CRT_SECURE_NO_WARNINGS");
			conf.Defines.Add("_CRT_NONSTDC_NO_DEPRECATE");

			if (target.Options == MSF_Options.Validation)
				conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP20);
			else
				conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP11);
		}

		[Configure(DevEnv.make)]
		public void ConfigureMake(Configuration conf, CrossTarget target)
		{
            conf.ProjectFileName = "[project.Name]_[target.Platform]_[target.Toolset]";

			conf.Options.Add(Options.Makefile.Compiler.Exceptions.Disable);
			conf.Options.Add(Options.Makefile.Compiler.Warnings.MoreWarnings);
			conf.Options.Add(Options.Makefile.Compiler.TreatWarningsAsErrors.Enable);

            if (target.Toolset == Toolset.Clang)
                conf.Options.Add(Options.Makefile.General.PlatformToolset.Clang);
            else
                conf.Options.Add(Options.Makefile.General.PlatformToolset.Gcc);

            if (target.Toolset == Toolset.GCov)
            {
                conf.AdditionalCompilerOptions.Add("-fprofile-arcs");
                conf.AdditionalCompilerOptions.Add("-ftest-coverage");
                conf.AdditionalLinkerOptions.Add("-fprofile-arcs");
                conf.AdditionalLinkerOptions.Add("-ftest-coverage");
            }

			if (target.Options == MSF_Options.Validation)
				conf.Options.Add(Options.Makefile.Compiler.CppLanguageStandard.Cpp2a);
			else
				conf.Options.Add(Options.Makefile.Compiler.CppLanguageStandard.Cpp11);
		}
	}

	[Generate]
	public class ModernStringFormatSolution : Solution
	{
		public ModernStringFormatSolution() : base(typeof(CrossTarget))
		{
			AddTargets(Globals.DefaultTargets);

			var root = Path.Combine(Util.GetCurrentSharpmakeFileInfo().DirectoryName, @"..\");

			ExtraItems[".github"] = new Strings { Util.DirectoryGetFiles(Path.Combine(root, ".github")) };
			ExtraItems["Scripts"] = new Strings { Util.DirectoryGetFiles(root, "*.bat") };
			ExtraItems["Scripts"] = new Strings { Util.DirectoryGetFiles(root, "*.sh") };
		}

		[Configure]
		public void ConfigureAll(Configuration conf, CrossTarget target)
		{
			conf.SolutionPath = "[solution.SharpmakeCsPath]" + Globals.PathToGen;

			conf.AddProject<ModernStringFormatProject>(target);
		}

		[Configure(DevEnv.VisualStudio)]
		public void ConfigureVS(Configuration conf, CrossTarget target)
		{
			conf.SolutionFileName = "[solution.Name]_[target.DevEnv]";
		}

		[Configure(DevEnv.make)]
        public void ConfigureMake(Configuration conf, CrossTarget target)
        {
            conf.SolutionFileName = "[solution.Name]_[target.Platform]_[target.Toolset]";
        }
    }

	public static class Main
	{
		[Sharpmake.Main]
		public static void SharpmakeMain(Sharpmake.Arguments args)
		{
			KitsRootPaths.SetKitsRoot10ToHighestInstalledVersion(DevEnv.vs2019);
			KitsRootPaths.SetKitsRoot10ToHighestInstalledVersion(DevEnv.vs2022);
			args.Generate<ModernStringFormatSolution>();
		}
	}
}