# Jinx

Jinx is a lightweight embedded scripting language, written in modern C++, and intended to be compiled and used from within a host application.  The Jinx language is clean and simple, looking like a cross between pseudo-code and natural language phrases, thanks to functions and identifiers that can incorporate whitespace. 

It was inspired by languages such as Lua, Python, C++, AppleScript, and a few others.  A significant design goal was to make Jinx not only simple to use as a language, but also to make it straightforward to integrate and use via the native API.  Jinx scripts are designed to be executed asynchronously as co-routines, or even on separate threads, since the runtime is thread-safe.  The language also uses no garbage collection, ensuring predictable CPU usage.  This makes it a potential candidate for real-time applications like videogames.

You can [visit the main Jinx website here](http://www.jinx-lang.org/), or [see recent changes](https://github.com/JamesBoer/Jinx/blob/master/Docs/Changelog.md)


