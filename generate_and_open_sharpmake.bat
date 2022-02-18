:: Cleanup excess solutions in-case we're downgrading
del Sharpmake\*.sln

call generate_projects.bat

set SHARPMAKE_SLN=sharpmake\sharpmake_debugsolution.vs2022.sln
if not exist %SHARPMAKE_SLN% set SHARPMAKE_SLN=sharpmake\sharpmake_debugsolution.vs2019.sln
if not exist %SHARPMAKE_SLN% set SHARPMAKE_SLN=sharpmake\sharpmake_debugsolution.vs2017.sln

start %SHARPMAKE_SLN%
