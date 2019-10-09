-keepnames class *

-keepclassmembers class * {*; }

-dontshrink

# Don't muck with test infra

-keep class org.fest.** { *; }
-keep class org.junit.** { *; }
-keep class junit.** { *; }
-keep class sun.misc.** { *; }
-keep class android.test.** { *; }
-keep class android.support.test.** { *; }
-keep class androidx.test.** { *; }

-optimizations !method/inlining/*

-dontwarn org.fest.**
-dontwarn org.junit.**
-dontwarn junit.**
-dontwarn sun.misc.**
-dontwarn android.test.**
-dontwarn android.support.test.**
-dontwarn androidx.test.**

-keepattributes SourceFile,LineNumberTable,Signature

-printmapping
-dontwarn android.content.**

-dontwarn org.xmlpull.v1.**
