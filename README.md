/*
Unreal Tournament 4 UE4 Hook

// gObjects
// 48 8D 0D ?? ?? ?? ?? C6

// gNames
// 48 89 1D ?? ?? ?? ?? 48 8B 5C 24 20
*/
An example drawing/aimbot hook for UE4 on Unreal Tournament 4

1) You must dump and fix up the SDK yourself if this current one stops working.
Included .zip (SDK.zip) with the SDK files.

2) Rotator aimbot works, but is linear with no velocity check.

3) No teamcheck, it's custom or undefined in this game. 

Exercises:
- see if you can use tags or other methods to place players onto teams. example. add friendly/foe arrays for aimbot
- see if you can add even a velocity check for aimbot to hit better over range, even if weapon info isn't available, you can hardcode some weapon projectile speeds and do linear math here pretty easily

Purpose:
The purpose is mostly to demonstrate how a hack could be made on any Unreal Engine 4 game in 64bit very quickly, even given the lack of references/info around for certain titles. 


