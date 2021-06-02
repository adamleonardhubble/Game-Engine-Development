-- Start Function. Run when the script component, or an object with this component attached, is first created
function Start()
	-- Initialize Stuff

	RayIndex = Raycasting.CreateRayFromMe(LuaVector.Create(0, 0, 0), LuaVector.Create(0, -1, 0))
	
end

-- Update Function. Run once per frame
function Update()

	HitInfo = Raycasting.GetRayHitInfo(RayIndex)

	if(HitInfo.DidHit) then

		Logger.Info("Hit: "..HitInfo.ObjectName)
	else
		Logger.Info("Hit Nothing")
	end

end