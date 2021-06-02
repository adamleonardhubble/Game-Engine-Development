-- Start Function. Run when the script component, or an object with this component attached, is first created
function Start()
	-- Define Global Variables
	Movespeed = 0.2
end

-- Update Function. Run once per frame
function Update()
	-- Get Input
	bKeyRight = Input.KeyPressed(Input.Keys.right)
	bKeyLeft = Input.KeyPressed(Input.Keys.left)

	HMove = 0.0

	-- Calculate Horizontal Move
	if (bKeyRight) then HMove = -1 end -- Moving Right?
	if (bKeyLeft)  then HMove = 1 end -- Moving Left?
	HMove = HMove * Movespeed -- Get Movement in Horrizontal Direction

	Transform.Position.x = Transform.Position.x + HMove
end