To build the project run make in the top level folder. This will build all the executables and output them to the bin/ folder. Cd into bin/ and run ./mysh. The commands mycat, myls, mycp and mysh my can be run without ./ within the shell because we set the path to include the bin folder.


You can use cd and pwd within the shell because they are "aliased" to the mycd and mypwd builtins.
