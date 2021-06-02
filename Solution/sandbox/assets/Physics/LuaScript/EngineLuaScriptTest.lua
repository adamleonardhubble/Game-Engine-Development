-- Start Function. Run when the script component, or an object with this component attached, is first created
function Start()
	-- Initialize Stuff
	--transform.scale.y = 2
	--local myObject = Object()
	--ridgidBody.applyForce(120, 120, 120) -- Apply Force
	--logger.info("fnfjnftjn")

	--RayIndex = Raycasting.CreateRayFromMe(0, 0, 0, 0, 0, 100)

	-- Create New Cube
	--CreateNew.Cube("TestCube", 0, 0, 0)

	-- Create Cube
	--MyCubePrefab = Prefab.Cube -- Define A Cube Prefab
	--CubePos = MyCubePrefab:GetPosition() -- Get Cube Position as Lua Vector
	--CubePos.x = -2 -- Move The Cube
	--MyCubePrefab:SetPosition(CubePos) -- Set Pos
	--CreateNew.FromPrefab(MyCubePrefab) -- Create Object From Prefab

	-- Create Cube
	MyCubePrefab = Prefab.Cube() -- Define A Cube Prefab
	MyCubePrefab.MaterialDiffuse = LuaColour.CreateFromHex("#4ee073") -- Define a Diffuse Material
	MyCubePrefab.Position = LuaVector.Create(5, 1, 1) -- Set Box Position
	MyCubePrefab:AddBoxCollider(LuaVector.Create(0.5, 0.5, 0.5), 1, 1) -- Add a Box Collider
	MyCubePrefab:SetCollidables(true, true, true, true) -- Set Collider Collidables
	MyCubePrefab:AddDynamicRigidBody(0.7, 0.15, 0.0, true) -- Add a Dynamic RidgidBody
	MyCubePrefab:AddLuaScript("assets/Physics/LuaScript/PrefabTest.lua") -- Add a Lua Script Component
	Prefab.Instantiate(MyCubePrefab) -- Create Object From Prefab
end

-- Update Function. Run once per frame
function Update()
	-- Rotate Model
	--value = 10 -- Set Value
	--logger.info("bfdbrdsbg")
	--logger.info("This is working fine") -- Output
	--transform.rotation.x = transform.rotation.x - (value * time.timestep) -- Rotate in X
	--transform.rotation.z = transform.rotation.y - (value * time.timestep) -- Rotate in Z
	--transform.rotation.y = transform.rotation.z - (value * time.timestep) -- Rotate in Y
	--model.update() -- Update Model
	--Object.XRot = Object.XRot - (value * Object.timestep)
	--Object.updatemodel()
	--myObject.XRot = myObject.XRot - (value)
	--local myObject = Object()
	--logger.info("grgreg")

	-- Raycast
	--HitInfo = Raycasting.GetRayHitInfo(RayIndex)
	--if(HitInfo.DidHit)
	--then
		--logger.info(HitInfo.ObjectName)
	--end
	--RayIndex = Raycasting.CreateRayFromMe(0, 0, 0, 0, 0, 100)
	--logger.info("Index 1: "..RayIndex)
end