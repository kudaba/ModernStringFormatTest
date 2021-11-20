using Sharpmake;
using System;

namespace ModernStringFormat
{
    [Fragment, Flags]
    public enum Toolset
    {
        Gcc = 1,
        Clang = 2,
        GCov = 4
    }

    public class CrossTarget : Target
    {
        public CrossTarget()
        {

        }
        public CrossTarget(Platform platform, DevEnv devEnv, Optimization optim, Toolset tools = Toolset.Clang)
            : base(platform, devEnv, optim)
        {
            Toolset = tools;
        }

        public Toolset Toolset;
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
			new CrossTarget(Platform.win64,
				DevEnv.vs2019,
				Optimization.Debug | Optimization.Release),
			new CrossTarget(Platform.linux,
				DevEnv.make,
				Optimization.Debug | Optimization.Release,
                Toolset.Gcc | Toolset.Clang | Toolset.GCov),
            new CrossTarget(Platform.mac,
                DevEnv.make,
                Optimization.Debug | Optimization.Release)
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
		}

		[Configure]
		public void ConfigureAll(Configuration conf, CrossTarget target)
		{
			conf.ProjectPath = "[project.SharpmakeCsPath]" + Globals.PathToGen;
			conf.ProjectFileName = "[project.Name]_[target.Platform]_[target.DevEnv]";

			conf.IncludePaths.Add("[project.SharpmakeCsPath]" + Globals.PathToSimpleTest);
			conf.IncludePaths.Add("[project.SharpmakeCsPath]" + Globals.PathToModernStringFormat);

			conf.IntermediatePath = "[project.SharpmakeCsPath]" + Globals.PathToBuild + @"\[target.Optimization]\[conf.ProjectFileName]";
			conf.TargetPath = "[project.SharpmakeCsPath]" + Globals.PathToBin;
			conf.TargetFileName = @"[conf.ProjectFileName]_[target.Optimization]";

			conf.Defines.Add("MSF_HEAVY_ASSERTS_ENABLED");
        }

        [Configure(DevEnv.VisualStudio)]
		public void ConfigureVS(Configuration conf, CrossTarget target)
		{
            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP11);
            conf.Options.Add(Options.Vc.Compiler.Exceptions.Disable);
			conf.Options.Add(Options.Vc.General.WarningLevel.Level4);
			conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);

			conf.Defines.Add("_CRT_SECURE_NO_WARNINGS");
			conf.Defines.Add("_CRT_NONSTDC_NO_DEPRECATE");
		}

		[Configure(DevEnv.make)]
		public void ConfigureMake(Configuration conf, CrossTarget target)
		{
            conf.ProjectFileName = "[project.Name]_[target.Platform]_[target.Toolset]";

            conf.Options.Add(Options.Makefile.Compiler.CppLanguageStandard.Cpp11);
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
        }
    }

	[Generate]
	public class ModernStringFormatSolution : Solution
	{
		public ModernStringFormatSolution() : base(typeof(CrossTarget))
		{
			AddTargets(Globals.DefaultTargets);
		}

		[Configure]
		public void ConfigureAll(Configuration conf, CrossTarget target)
		{
			conf.SolutionPath = "[solution.SharpmakeCsPath]" + Globals.PathToGen;
			conf.SolutionFileName = "[solution.Name]_[target.Platform]_[target.DevEnv]";

			conf.AddProject<ModernStringFormatProject>(target);
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
			args.Generate<ModernStringFormatSolution>();
		}
	}
}