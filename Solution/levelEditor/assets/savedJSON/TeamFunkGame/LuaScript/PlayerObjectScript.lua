-- Start Function. Run when the script component, or an object with this component attached, is first created
function Start()
	-- Define Global Variables
	Movespeed = 5
	JumpForce = 35

	RayIndex = Raycasting.CreateRayFromMe(LuaVector.Create(0, 0, 0), LuaVector.Create(0, -0.6, 0))
end

-- Update Function. Run once per frame
function Update()

	HitInfo = Raycasting.GetRayHitInfo(RayIndex)

	HMove = 0

	if (Input.KeyPressed(Input.Keys.d)) then HMove = -1 end
	if (Input.KeyPressed(Input.Keys.a)) then HMove = 1 end

	HMove = HMove * Movespeed

	RigidBody.Velocity.Set(LuaVector.Create(HMove, RigidBody.Velocity.y, 0))


	if(HitInfo.DidHit) then

		Logger.Info("Hit: "..HitInfo.ObjectName)

		if (Input.KeyPressed(Input.Keys.space)) then

			RigidBody.ApplyForce(LuaVector.Create(0, JumpForce, 0))
		end
	else
		Logger.Info("Hit Nothing")
	end
end

-- OnCollision Function. Called Once When a Collision First Happens
function OnCollision()
	-- Output
	--sName = CollisionData.OtherName
	--logger.info("Collided Object Name: " .. sName)
end