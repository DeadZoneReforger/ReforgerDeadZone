
//------------------------------------------------------------------------------------------------
//! Base class for Prefabs Spawning.
[BaseContainerProps()]
class SCR_LAGPrefabsSpawner
{
	
	[Attribute("5", UIWidgets.Slider, "Frequency in minutes to recalculate new item spawn", "0 30")]
    protected int m_iMinutesToRecalcutate;
	
	[Attribute("90", UIWidgets.Slider, "Frequency in minutes to clean up respawn cache. Should be happens in the middle of server runtime", "0 240")]
    protected int m_iMinutesToCleanUpCache;
	
	[Attribute("0", UIWidgets.EditComboBox, "Defines which spawner uses this Spawn Point", "", ParamEnumArray.FromEnum(E_DZ_PrefabSpawnType))]
	protected E_DZ_PrefabSpawnType m_eType;
	
	[Attribute("0", uiwidget: UIWidgets.Flags, "Game Flags required to spawn contained prefabs.", "", ParamEnumArray.FromEnum(EGameFlags))]
	protected EGameFlags m_eRequiredGameFlags;
	
	[Attribute("50", UIWidgets.Slider, "Percentage of how many prefabs to spawn based on how many SCR_PrefabSpawnPoints(with defined type) exists in Mission.", "0 100")]
    protected int m_iTotalPrefabs;
	
	[Attribute("", UIWidgets.Object, "Add categories of prefabs and/or specific prefabs to spawn.")]
	protected ref array<ref SCR_LAGBasePrefabCategory> m_aPrefabsCategories;

	//! List of all SCR_PrefabSpawnPoints for specific E_DZ_PrefabSpawnType
	protected ref array<SCR_LAGPrefabSpawnPoint> m_aPrefabSpawnPoints;
	
	//! List of all SCR_PrefabSpawnPoints for specific E_DZ_PrefabSpawnType
	protected ref array<SCR_LAGPrefabSpawnPoint> m_aFreePrefabSpawnPoints = new array<SCR_LAGPrefabSpawnPoint>();
	
	//------------------------------------------------------------------------------------------------
	E_DZ_PrefabSpawnType GetType()
	{
		return m_eType;
	}
	
	//------------------------------------------------------------------------------------------------
	void ActivateRespawn(array<SCR_LAGPrefabSpawnPoint> prefabSpawnPoints, int delayMS)
	{
		if(m_iMinutesToRecalcutate > 0)
		{
			GetGame().GetCallqueue().CallLater(Spawn, ToMilliseconds(m_iMinutesToRecalcutate) + delayMS, true, prefabSpawnPoints, true);
		}
		
		if(m_iMinutesToCleanUpCache > 0 && m_iMinutesToRecalcutate > 0 )
		{
			GetGame().GetCallqueue().CallLater(cleanUPForNewRespawn, ToMilliseconds(m_iMinutesToCleanUpCache), true);	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Call this to trigger spawn logic for this spawner
	void Spawn(array<SCR_LAGPrefabSpawnPoint> prefabSpawnPoints, bool isRespawnMode = false)
	{
		
		
		if (m_eRequiredGameFlags != 0 && !GetGame().AreGameFlagsSet(m_eRequiredGameFlags))
		{
			return;
		}
		
		if (prefabSpawnPoints == null)
		{
			
			//Debug.Error("Can't spawn items of type: " + m_eType.ToString() + " because no spawn points are available.");
			return;
		}
		
		m_aPrefabSpawnPoints = prefabSpawnPoints;
		
		int numberOfPrefabsToSpawn = m_aPrefabSpawnPoints.Count() * m_iTotalPrefabs / 100 ;
			
		if(isRespawnMode == false)
		{	
			Print(string.Format("DZ::RS::(type: %1) Start initial prefab load", m_eType.ToString()), LogLevel.WARNING);
			Print(string.Format("DZ::RS::(type: %1) Sum Spawnpoints: %2",  m_eType.ToString(), m_aPrefabSpawnPoints.Count()), LogLevel.WARNING);
			Print(string.Format("DZ::RS::(type: %1) Initial load with: %2 prefabs", m_eType.ToString(), numberOfPrefabsToSpawn), LogLevel.WARNING);
		}
		
		SpawnPrefabsCategories(numberOfPrefabsToSpawn, m_aPrefabsCategories, isRespawnMode);
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! Trigers spawning for Categories of prefabs and/or nested categories of prefabs.
	void SpawnPrefabsCategories(int numberOfPrefabsToSpawn, array<ref SCR_LAGBasePrefabCategory> prefabsCategories, bool isRespawnMode = false)
	{
		
		int totalRatio = GetTotalRatio(prefabsCategories);
		
		if (totalRatio == 0)
		{
			return;
		}
		
		foreach (SCR_LAGBasePrefabCategory prefabCategory : prefabsCategories)
		{
			
			prefabCategory.Spawn(numberOfPrefabsToSpawn * prefabCategory.GetRatio() / totalRatio, this, isRespawnMode);
		}
		
		if(isRespawnMode == false)
		{
			//Print(string.Format("DZ::RS::(type: %1) Total Categories: %2", m_eType.ToString(), prefabsCategories.Count()), LogLevel.WARNING);
			InitFreeSpawnPoints();
		}
		
	}
	
	//------------------------------------------------------------------------------------------------
	//! Spawn prefabs on random spawnpoints.
	void SpawnPrefabCategory(int totalprefabs, ResourceName prefab)
	{
		if (totalprefabs <= 0 || m_aPrefabSpawnPoints == null)
		{
			return;
		}
		
		for (int i = 0; i < totalprefabs; i++)
		{
			SCR_LAGPrefabSpawnPoint prefabSpawnPoint = m_aPrefabSpawnPoints.GetRandomElement();
			
			prefabSpawnPoint.hasSpawnedItems = true;
			prefabSpawnPoint.spawnCount++;
			
			vector mat[4];
			prefabSpawnPoint.GetWorldTransform(mat);
			
			if (prefabSpawnPoint.ShouldSnapToGound())
			{
				vector position = mat[3];
				position[1] = prefabSpawnPoint.GetWorld().GetSurfaceY(position[0], position[2]);
				mat[3] = position;
			}
				
			EntitySpawnParams spawnParams = new EntitySpawnParams;
			spawnParams.TransformMode = ETransformMode.WORLD;
			spawnParams.Transform = mat;
			
			Resource resource = Resource.Load(prefab);
			IEntity entity = GetGame().SpawnEntityPrefab(resource, null, spawnParams);
		}
		
		
	}
	
	void InitFreeSpawnPoints()
	{
		foreach(SCR_LAGPrefabSpawnPoint spawnPoint : m_aPrefabSpawnPoints)
		{	
			if(spawnPoint.hasSpawnedItems)
			{
				continue;
			}
			
			m_aFreePrefabSpawnPoints.Insert(spawnPoint);
		
		}
		///Print(string.Format("DZ::RS::(type: %1) Available Spawnpoints after initial load: %2", m_eType.ToString(), m_aFreePrefabSpawnPoints.Count()), LogLevel.WARNING);
	
	}
	
	//------------------------------------------------------------------------------------------------
	void ReSpawnPrefabCategory(int totalprefabs, ResourceName prefab)
	{
		
		if(m_aFreePrefabSpawnPoints.Count() == 0)
		{
			///Print(string.Format("DZ::RS::(type: %1) No Spawnpoints available, waiting for cache cleanUp...", m_eType.ToString()), LogLevel.WARNING);
			return;
		}
		
		///Print(string.Format("DZ::RS::(type: %1) Respawn new item (available Spawnpoints: %2)", m_eType.ToString(), m_aFreePrefabSpawnPoints.Count()), LogLevel.WARNING);
		SCR_LAGPrefabSpawnPoint freeSpawnPoint = m_aFreePrefabSpawnPoints.GetRandomElement();
		
		m_aFreePrefabSpawnPoints.RemoveItem(freeSpawnPoint);
		if(!freeSpawnPoint)
		{
				return;
		}
		
		
		vector mat[4];
		freeSpawnPoint.GetWorldTransform(mat);
		
		if (freeSpawnPoint.ShouldSnapToGound())
		{
			vector position = mat[3];
			position[1] = freeSpawnPoint.GetWorld().GetSurfaceY(position[0], position[2]);
			mat[3] = position;
		}
			
		EntitySpawnParams spawnParams = new EntitySpawnParams;
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform = mat;
		
		Resource resource = Resource.Load(prefab);
		IEntity entity = GetGame().SpawnEntityPrefab(resource, null, spawnParams);
	}
	
	
	//------------------------------------------------------------------------------------------------
	void cleanUPForNewRespawn()
	{
		///Print(string.Format("DZ::RS::(type: %1) Cleaning cache for new spawnpoints", m_eType.ToString() ), LogLevel.WARNING);
		m_aFreePrefabSpawnPoints =  new array<SCR_LAGPrefabSpawnPoint>();
		
		foreach( SCR_LAGPrefabSpawnPoint sp: m_aPrefabSpawnPoints)
		{
			sp.hasSpawnedItems = false;
			m_aFreePrefabSpawnPoints.Insert(sp);
		}
		///Print(string.Format("DZ::RS::(type: %1) New available spawnpoints: %2", m_eType.ToString(), m_aFreePrefabSpawnPoints.Count() ), LogLevel.WARNING);
	}
		
	
	//------------------------------------------------------------------------------------------------
	//! Total Ratio for prefab/s categories on same level of hierarchy.
	protected int GetTotalRatio(array<ref SCR_LAGBasePrefabCategory> prefabsCategories)
	{
		int totalWeight = 0;
		foreach (SCR_LAGBasePrefabCategory prefabCategory : prefabsCategories)
		{
			totalWeight += prefabCategory.GetRatio();
		}
		
		return totalWeight;
	}
	
	//------------------------------------------------------------------------------------------------
	int ToMilliseconds(int minutes)
	{
		return minutes * 1000 * 60;
	}	
	
	//------------------------------------------------------------------------------------------------
	void SCR_LAGPrefabsSpawner()
	{
		
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_LAGPrefabsSpawner()
	{
		
	}
};