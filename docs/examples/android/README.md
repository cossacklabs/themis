# Android example 

## Compile 
```
./gradlew clean build
```

## Install 
```
./gradlew installDebug
```
You should see the empty screen with the message "Hello from themis!" and title "ThemisTestApp". 
Also, you may to run `adb logcat | grep SMC` and see something like this:
```agsl
10-03 21:36:57.842 16956 16956 D SMC     : encrypted string = AAEBQQwAAAAQAAAADQAAABYAAAASizR1/IPS4UYioLPeH8VpdgvaXl9875aZorAZ/soEABAAa/ARqQwo6liyQrluj06zohARxU/0LBNYb3UvKAc=
10-03 21:36:58.776 16956 16956 D SMC     : decrypted data = hello message
```
