#pragma once

// Base COntroller for the Engine and Sandbox Game.

class Controller
{
public:
	Controller ();
	virtual ~Controller () = default;
	
	// called Every Frame
	virtual void Update (float deltaTime) = 0;
};

