在linux下使用cmake && make && ./MOG可编译运行该程序

在windows下使用cmake会生成Visual Studio工程文件，可以使用devenv .\MOG.sln /Build "Release|x64"
命令编译，也可以直接使用Visual Studio打开MOG.sln。

MOG.exe运行后要求输入视频路径，可以将视频放在同一文件夹下，并输入其相对路径，即文件名。结果将输出为result.avi。