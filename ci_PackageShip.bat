//C:\UnrealEngine\Engine\Build\BatchFiles\Build.bat Canyon Win64 Shipping -project=C:\ueproj\Canyon.uproject
//docker run -v "host path":"C:\ueproj" ...

C:\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat BuildCookRun -platform=Win64 -clientconfig=Shipping -serverconfig=Shipping -noP4 -cook -allmaps -build -stage -prereqs -pak -archieve -archivedirectory=C:\ueproj\packaged -project=C:\ueproj\Canyon\Canyon.uproject
