//------------------------------------------------------------------------------------------------
//! Prefab to be spawned by SCR_PrefabsSpawnerManager.
[BaseContainerProps()]
class SCR_LAGPrefabCategory : SCR_LAGBasePrefabCategory
{
	[Attribute( "", UIWidgets.ResourcePickerThumbnail, "Contained Prefab", "et" )]
	ResourceName m_Prefab;
	
	//------------------------------------------------------------------------------------------------
	override void Spawn(int numberOfPrefabsToSpawn, SCR_LAGPrefabsSpawner prefabsSpawner, bool isRespawnMode = false)
	{
		if(isRespawnMode == false)
		{
			prefabsSpawner.SpawnPrefabCategory(numberOfPrefabsToSpawn, m_Prefab);
			return;
		}
		
		prefabsSpawner.ReSpawnPrefabCategory(numberOfPrefabsToSpawn, m_Prefab);
		
	}
};