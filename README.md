OpenIAB-Marmalade-Plugin
========================

OpenIAB plugin for [Marmalade SDK](https://www.madewithmarmalade.com/products/marmalade-sdk)

Integration
-----------

1) Build plugin

* Java library
```
mkb extension/MyExtension/MyExtension_android_java.mkb
```
* Native library
```
mkb extension/MyExtension/MyExtension_android.mkb --debug
mkb extension/MyExtension/MyExtension_android.mkb --release 
```

2) Include ```openiab.mkf``` to the application ```.mkb``` file.

```
subprojects
{
  ../../marmalade_plugin/openiab.mkf
}
```

Please refer to [Marmalade EDK documentation](http://docs.madewithmarmalade.com/display/MD741/Using+and+updating+extensions) for details.

Build and run application
-----
1) Setup everything required in the dependency checker

![Dependency checker](http://take.ms/IZumK)

2) Build the project in the Marmalade Hub

![Hub build and run](http://take.ms/Qvzth)
