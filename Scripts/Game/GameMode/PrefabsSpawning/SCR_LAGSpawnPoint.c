[EntityEditorProps(style: "cylinder", category: "GameScripted/GameMode/PrefabsSpawn", description: "Prefab Spawn Point Entity", sizeMin: "-0.25 0 -0.25", color: "64 0 64 255")]
class SCR_LAGPrefabSpawnPointClass: GenericEntityClass
{
};

//------------------------------------------------------------------------------------------------
//! Prefab Spawn Point Entity defines position where prefab can be spawned.
class SCR_LAGPrefabSpawnPoint : GenericEntity
{
	[Attribute("0", UIWidgets.EditComboBox, "Defines which spawner uses this Spawn Point", "", ParamEnumArray.FromEnum(E_DZ_PrefabSpawnType) )]
	protected E_DZ_PrefabSpawnType m_eType;
	
	[Attribute("0", UIWidgets.CheckBox, "Should be SP snaped in script to ground? -- GetWorld().GetSurfaceY()", "", ParamEnumArray.FromEnum(E_DZ_PrefabSpawnType) )]
	protected bool m_bSnapToGroud;
	
	bool hasSpawnedItems = false;
	
	int spawnCount = 0;
	
	//------------------------------------------------------------------------------------------------
	E_DZ_PrefabSpawnType GetType()
	{
		return m_eType;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ShouldSnapToGound()
	{
		return m_bSnapToGroud;
	}
	
	//------------------------------------------------------------------------------------------------
	void SCR_LAGPrefabSpawnPoint(IEntitySource src, IEntity parent)
	{
		#ifdef WORKBENCH
			SetFlags(EntityFlags.TRACEABLE, false);
		#else
			SetFlags(EntityFlags.NO_LINK, false);
		#endif
		SCR_LAGPrefabsSpawnerManager.RegisterPrefabSpawnPoint(this);
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_LAGPrefabSpawnPoint()
	{
		SCR_LAGPrefabsSpawnerManager.UnregisterPrefabSpawnPoint(this);
	}
};