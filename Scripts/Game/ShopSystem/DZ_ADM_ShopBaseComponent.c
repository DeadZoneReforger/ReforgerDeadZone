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
		if (!Replication.IsServer())
			return false;
		
		SCR_InventoryStorageManagerComponent plyInventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_WalletPrefab));
		
		bool canSell = merchandise.GetType().CanSell(player, this, merchandise, quantity);
		if (!canSell) 
		{
			playerManager.SetTransactionMessage("Error! You do not have the required merchandise to sell!");
			return false;
		}

		bool canCollect = merchandise.GetType().CanCollectMerchandise(player, this, merchandise, quantity); ///returns canSell 
		if (!canCollect)
		{
			
			playerManager.SetTransactionMessage("Error! Merchandise could not be sold!");
			return false;
		}
		
		bool shouldRemoveWallet = false; 
		if (IsSellPaymentOnlyCurrency(merchandise)) ///check if the item being sold uses currency
		{
			array<IEntity> currencyItems = ADM_CurrencyComponent.FindCurrencyInInventory(plyInventory); // thanks to Lag from plxyable for the cast option
			if (currencyItems.Count() <= 0)
			{
				//Print("(-) DZ: No Wallet Found");
				
				vector mat[4];
				player.GetTransform(mat);
				item.SetTransform(mat);
				bool insertResult = ADM_Utils.InsertAutoEquipItem(plyInventory, item);

				if (!insertResult)
				{
					//Print("(-) DZ: Couldnt add wallet");
					playerManager.SetTransactionMessage("Error! You don't have room for the wallet.");
					return false;
				}
				 shouldRemoveWallet = true; /// player didnt have a wallet
			}
				
		} 	/// does wallet check before removing items 
		
		bool didCollect = merchandise.GetType().CollectMerchandise(player, this, merchandise, quantity);
		if (!didCollect) /// remove the items being sold if it fails returns items and removes wallet if we gave it to the player
		{
			if(shouldRemoveWallet) /// player didnt have a wallet before selling so remove the one we gave them if we fail to sell items
			{
				bool removedWallet = plyInventory.TryDeleteItem(item);
				//Print("DZ [removedWallet] " + removedWallet);
			}
			
			playerManager.SetTransactionMessage("Error! Merchandise could not be collected!");
			return false;
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