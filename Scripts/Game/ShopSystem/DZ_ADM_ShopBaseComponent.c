modded class ADM_ShopBaseComponentClass: ScriptComponentClass {}
modded class ADM_ShopBaseComponent: ScriptComponent
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Empty Wallet Prefab", "et",)]
	protected ResourceName m_WalletPrefab;
	
	  override static bool IsSellPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetSellPayment();
		return (requiredPayment.Count() == 1 && requiredPayment[0].Type() == ADM_PaymentMethodCurrency);
	}
	
	
	override bool AskSell(IEntity player, ADM_PlayerShopManagerComponent playerManager, ADM_ShopMerchandise merchandise, int quantity)
	{
		IEntity emptyWallet = GetGame().SpawnEntityPrefab(Resource.Load(m_WalletPrefab));	
		if (!Replication.IsServer())
			return false;
		
		bool canSell = merchandise.GetType().CanSell(player, this, merchandise, quantity);
		if (!canSell) 
		{
			playerManager.SetTransactionMessage("Error! You do not have the required merchandise to sell!");
			return false;
		}
		
		bool canCollect = merchandise.GetType().CanCollectMerchandise(player, this, merchandise, quantity);
		if (!canCollect)
		{
			playerManager.SetTransactionMessage("Error! Merchandise could not be sold!");
			return false;
		}
		
		bool didCollect = merchandise.GetType().CollectMerchandise(player, this, merchandise, quantity);
		if (!didCollect)
		{
			playerManager.SetTransactionMessage("Error! Merchandise could not be collected!");
			return false;
		}
		if (IsSellPaymentOnlyCurrency(merchandise)) ///check if the item being sold uses currency
		{
			array<IEntity> items = {};  /// array for players inventory
			SCR_InventoryStorageManagerComponent plyInventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
			plyInventory.GetItems(items);
			bool hasWallet false;
			
		foreach (IEntity item : items) /// loop through players items array
		{
			if (item.GetPrefabData().GetPrefabName() == emptyWallet.GetPrefabData().GetPrefabName()) ///check if the the player has the empty wallet prefab
			{
				///Print("DZ [FindWallet] Wallet Found");
				hasWallet = true;
				break;
			}
		}
		if (!hasWallet) //checks if the player already has a wallet or not if they dont try to add one
		{

				bool putWalletIn = ADM_Utils.InsertAutoEquipItem(plyInventory, emptyWallet);
				if(!putWalletIn) /// couldnt fit/add the wallet
				{
					playerManager.SetTransactionMessage("Error! Cash couldnt be added!");
					return false;
				}
		}
				
		} 	
		foreach (ADM_PaymentMethodBase paymentMethod : merchandise.GetSellPayment())
		{
			bool returnedPayment = paymentMethod.DistributePayment(player, quantity);
			if (!returnedPayment)
			{
				PrintFormat("Error giving payment! %1", paymentMethod.Type().ToString());
			}
		}
		playerManager.SetTransactionMessage("Success! Merchandise sold.");
		
		// Invoke OnPostSell (event after a purchase is completed for gamemodes to hook to)
		// Pass in the shop, player who purchased, and sold item
		Event_OnPostSell.Invoke(this, player, null);
		
		return true;
	}	
}