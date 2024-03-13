enum E_DZ_PrefabSpawnType
{
    ItemCiv,
    ItemMilitary,
    VehicleCiv,
    Wrecks,
    WreckCrates
};

//------------------------------------------------------------------------------------------------
class SCR_LAGPrefabsSpawnerManagerClass: GenericEntityClass
{
};

//------------------------------------------------------------------------------------------------
//! Manager for spawning prefabs in the mission.
class SCR_LAGPrefabsSpawnerManager : GenericEntity
{

	[Attribute("", UIWidgets.Object, "Prefabs spawners, each for different type of SpawnPoints")]
	protected ref array<ref SCR_LAGPrefabsSpawner> m_aPrefabsSpawners;

	//! Static lists of all SCR_PrefabSpawnPoints
	protected static ref map<E_DZ_PrefabSpawnType, ref array<SCR_LAGPrefabSpawnPoint>> s_aPrefabSpawnPoints = new map<E_DZ_PrefabSpawnType, ref array<SCR_LAGPrefabSpawnPoint>>();

	//------------------------------------------------------------------------------------------------
	static void RegisterPrefabSpawnPoint(SCR_LAGPrefabSpawnPoint prefabSpawnPoint)
	{
		if (GetGame().GetWorldEntity() && s_aPrefabSpawnPoints)
		{
			array<SCR_LAGPrefabSpawnPoint> spawnPoints = s_aPrefabSpawnPoints[prefabSpawnPoint.GetType()];

			if (!spawnPoints)
			{
				spawnPoints = new array<SCR_LAGPrefabSpawnPoint>();
				s_aPrefabSpawnPoints.Insert(prefabSpawnPoint.GetType(), spawnPoints);
			}

			spawnPoints.Insert(prefabSpawnPoint);
		}
	}

	//------------------------------------------------------------------------------------------------
	static void UnregisterPrefabSpawnPoint(SCR_LAGPrefabSpawnPoint prefabSpawnPoint)
	{
		if (GetGame().GetWorldEntity() && s_aPrefabSpawnPoints)
		{
			array<SCR_LAGPrefabSpawnPoint> spawnPoints = s_aPrefabSpawnPoints[prefabSpawnPoint.GetType()];
			if (spawnPoints)
				spawnPoints.RemoveItem(prefabSpawnPoint);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Called only once. All SCR_PrefabSpawnPoints are already registered.
	override void EOnFrame(IEntity owner, float timeSlice)
	{

        if (!GetGame().GetWorldEntity() || RplSession.Mode() == RplMode.Client)
        {
			//Print("DZ::RS::InRPL client mode....", LogLevel.WARNING);
            ClearEventMask(EntityEvent.FRAME);

            return;
        }


		DeadZoneCore.InitLogFolders(SCR_DateTimeHelper.GetDateTimeUTC());

		string.Format("PrefapSpawner {0}", m_aPrefabsSpawners);
		int delayMS = 5000;
        foreach (auto prefabsSpawner : m_aPrefabsSpawners)
        {

			prefabsSpawner.Spawn(s_aPrefabSpawnPoints[prefabsSpawner.GetType()], false);
            prefabsSpawner.ActivateRespawn(s_aPrefabSpawnPoints[prefabsSpawner.GetType()], delayMS);
			delayMS += 5000;
        }

        ClearEventMask(EntityEvent.FRAME);
	}


	//------------------------------------------------------------------------------------------------
	void SCR_LAGPrefabsSpawnerManager(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.FRAME);
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_LAGPrefabsSpawnerManager()
	{
		s_aPrefabSpawnPoints = new map<E_DZ_PrefabSpawnType, ref array<SCR_LAGPrefabSpawnPoint>>();
	}
};
