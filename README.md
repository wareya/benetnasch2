benetnasch2
===========
Benetnasch2 is a basic platformer engine written in C++/SDL. Absolutely everything is implemented from scratch, with a very small number of libraries used:

* Faucet Networking, which uses Boost, provides networking capabilities
* Faucet Mixer, which uses SDL, provides games audio functionality
* Lua provides scripting capabilities for things like the HUD

Networking uses UDP with a checkpoints system to prevent particular kinds of out-of-order message. The general paradigm is extrapo-prediction.

The character movement solver is extremely well-responding to very dynamic framerates. Character collision detection is floatingpoint-perfect.

Benetnasch compiles and works properly on windows and linux. Other platforms have not been tested, but almost definitely work.
