#include <amxmodx>
#include <amxmisc>
#include <cstrike>
#include <fun>
#include <hamsandwich>
#include <fakemeta>
#include <fakemeta_util>
#include <engine>

new bool:userHelpAdmin[33];
new userPageHelp[33]

new needHelp[33]
new menuList[33][33]
new Float:userTimeAdmin[33];

new viewBlock[33]
new bool:selectBlock[33];

new typeMoveBlock[33];

new lightCharacter[] = "abcdefghijklmnopqrstuvwxyz";
new lightType[2];


public mainMenuAdmin(id){
	
	if(!playerLogged(id) || !isAdmin(id)){
		mainMenuAccount(id)
		return PLUGIN_CONTINUE;
	}
	
	new gText[1756], iLen

	iLen = format(gText[iLen], sizeof(gText)-iLen-1, "\r[BaseBuilder]\y Menu Admina:^n")

	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r1.\y NoClip:\w [ %s%s\w ]", userNoClip[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r2.\y GodMod:\w [ %s%s\w ]", userGodMod[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r3.\y Budowanie:\w [ %s%s\w ]", userAllowBuild[id] ? "\r" : "\d",symbolsCustom[SYMBOL_DOT]);
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r4.\y Odpychanie:\w [ %s%s\w ]^n", userPush[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	
	new iNum = 0;
	for( new i = 0; i < sizeof(needHelp); i ++ ){
		if( needHelp[i] == 0 )
			break
		if( needHelp[i] == -1 )
			continue
		iNum++;
	}

	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r5.%s Pomoc%s (%d Graczy)", iNum > 0 ?"\y":"\d",iNum > 0 ?"\r":"\d",  iNum)
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r6.\w Przeszuwasz jako:\r %s^n", userMoveAs[id] ? userName[userMoveAs[id]] : userName[id]);
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r7.\y Uprawnienia\d ( Zacznij Pomagac )");
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r8.\y Zmiana druzyny");
	
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r9.\y Ustawienia^n");
	
	
	
	
	iLen += format(gText[iLen], sizeof(gText)-iLen-1, "^n\r0.\w Wyjdz")
	
	show_menu(id, B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8 | B9 | B0, gText, -1, "mainMenuAdmin"  )
	return PLUGIN_HANDLED;

	
}

public mainMenuAdmin_2(id, item){
	
	switch(item){
		case 0:{
			userNoClip[id] =! userNoClip[id]
			set_user_noclip(id, userNoClip[id]);
			mainMenuAdmin(id)
		}
		case 1:{
			userGodMod[id] =! userGodMod[id]
			set_user_godmode(id, userGodMod[id]);
			mainMenuAdmin(id)
		}
		case 2:{
			userAllowBuild[id] =! userAllowBuild[id]	
			if( !userAllowBuild[id] ){
				stopEnt(id)
			}
			mainMenuAdmin(id)
		}
		case 3:{
			if(buildTime || prepTime){
				ColorChat(id, GREEN, "---^x01 Funkcja^x03 dostepna^x01 tylko podczas rundy!^x04 ---");
				mainMenuAdmin(id)
				return PLUGIN_CONTINUE;
			}
			userPush[id] =! userPush[id]	
			
			if( task_exists(id+TASK_PUSH) && userPush[id] )
				remove_task(id+TASK_PUSH)
			else set_task(0.1, "adminHelpPush", id+TASK_PUSH)
			
			mainMenuAdmin(id)
		}
		case 4:{
			if( menuAdminHelp(id) == 0 ){				
				ColorChat(id, GREEN, "---^x01 Nikt nie potrzebuje pomocy^x04 ---")	
				mainMenuAdmin(id)
			}
		}
		case 5:{
			userMenuPlayer[id] = MENU_VIEW_BLOCK
			choosePlayer(id, 0)	
		}
		case 6:{

			selectAimingMoveAs(id);
			if (id != userMoveAs[id] && userMoveAs[id]){
				menuPerrmissions(id);
			}else{
				ColorChat(id, GREEN, "---^x01 Pierw najedz na^x03 gracza^x01 lub na^x03 blok^x01 gracza!^x04 ---");
				mainMenuAdmin(id);
			}
		}
		case 7:{
			userMenuPlayer[id] = MENU_CHANGE_TEAM
			choosePlayer(id, 0)
			
		}
		case 8:{
			settingAdmin(id)
		}
		
		default:{return PLUGIN_HANDLED;}
	}
	
	if(userNoClip[id] || userGodMod[id] || userAllowBuild[id] || userPush[id]){
		set_rendering(id, kRenderFxGlowShell, accentColorHud[0], accentColorHud[1], accentColorHud[2], kRenderNormal, 5);		
	} else set_rendering(id, kRenderFxGlowShell, 0, 0, 0, kRenderNormal, 0);
	
	return PLUGIN_CONTINUE;
}

public selectAllBlock(id){
	
		
	new szClass[10], szTarget[10];
		
	for(new ent = maxPlayers; ent < 1024; ent  ++){
						
		if( !pev_valid(ent) )
			continue;	
		if(getOwner(ent) != viewBlock[id])
			continue;
		
							
		entity_get_string(ent, EV_SZ_classname, szClass, sizeof(szClass));
		entity_get_string(ent, EV_SZ_targetname, szTarget, sizeof(szTarget));
						
		if( !equal(szClass, "func_wall") || containi(szTarget, "ignore")!=-1 || containi(szTarget, "JUMP")!=-1)
			continue;	
		
		if(selectBlock[id]){
			set_pev(ent, pev_rendermode,kRenderTransColor)
			set_pev(ent,pev_rendercolor, Float:{255.0, 37.0, 37.0} )
			set_pev(ent,pev_renderamt, 255.0 )
		} else {
			set_pev(ent,pev_rendermode,kRenderNormal)
			set_pev(ent,pev_rendercolor,0.0,0.0,0.0)
			set_pev(ent,pev_renderamt,255.0)
		}			
	}
}



public settingAdmin(id){
	new menu = menu_create("\r[BaseBuilder]\y Ustawienia:", "settingAdmin_2")
	new gText[128];	
	menu_additem(menu, "Przywroc Klocki")
	menu_additem(menu, "Usun Klocki")
	
	format(gText, sizeof(gText), "Odliczanie: %s^n^n", !clockStop ? "\rNormalne" : "\dZatrzymane");
	menu_additem(menu, gText)

	if(isSuperAdmin(id)){
	
		menu_additem(menu, "Bloki gracza")
		menu_additem(menu, "\rHasla");
		menu_additem(menu, "\yPrzywroc");
	}
	menu_display(id, menu, 0)
}
	 
public settingAdmin_2(id, menu, item){
	if( item == MENU_EXIT){
		menu_destroy(menu)
		mainMenuAdmin(id)
		return PLUGIN_CONTINUE
	}
	
	switch(item){
		case 0:{
			adminResetBlock(1, 0, 0);
			mainMenuAdmin(id)
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 przywrocil^x04 klocki^x01 na mapie^x04 ---", userName[id]);
			settingAdmin(id)
		}
		case 1:{
			adminResetBlock(0, 0, 0);
			mainMenuAdmin(id)
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 usunal^x04 klocki^x01 z mapy^x04 ---",  userName[id]);
			settingAdmin(id)
		}
		case 2:{
			clockStop =! clockStop;
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 %s odliczanie^x04 ---",  userName[id], clockStop ? "zatrzymal" : "wznowil");
			settingAdmin(id)
		}
		case 3:{
			if(!isSuperAdmin(id)){
				mainMenuAdmin(id)
				return PLUGIN_CONTINUE;	
			}

			new ent; 
			get_user_aiming(id, ent)
		
			if(getOwner(ent) == 0){
				ColorChat(id, GREEN, "%s Nie mozesz tego zaznaczyc!", PREFIXSAY);
				settingAdmin(id)
				return PLUGIN_CONTINUE;	
			}
			if(!canBeMoved(id, ent)){
				ColorChat(id, GREEN, "%s Nie mozesz tego zaznaczyc!", PREFIXSAY);
				settingAdmin(id)
				return PLUGIN_CONTINUE;
			}
			viewBlock[id] = getOwner(ent);
			selectAllBlockMenu(id)
			
		}
		case 4:{
			if(!isSuperAdmin(id)){
				mainMenuAdmin(id)
				return PLUGIN_CONTINUE;	
			}
			userMenuPlayer[id] = MENU_PASSWORD
			choosePlayer(id, 0)
		}
		case 5:{
			if(!isSuperAdmin(id)){
				mainMenuAdmin(id)
				return PLUGIN_CONTINUE;	
			}
			userMenuPlayer[id] = MENU_PLAYER_CAMP
			choosePlayer(id, 0)
		}
	}
	return PLUGIN_CONTINUE
}

new timeHelpGod = 20;

public menuPerrmissions(id){
	
	new gText[512]

	new target = userMoveAs[id];

	format(gText, sizeof(gText), "\r[BaseBuilder]\y Pomagasz graczowi:\r %s\d", userName[target]);
	
	new menu = menu_create(gText, "menuPerrmissions_2");
	
	format(gText, sizeof(gText), "\yNoClip:\w [ %s%s\w ]", userNoClip[target] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	format(gText, sizeof(gText), "\yGodMod:\w [ %s%s\w ]", userGodMod[target] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	format(gText, sizeof(gText), "\yBudowanie:\w [ %s%s\w ]^n^n\d[Twoje Atrybuty]", userAllowBuild[target] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	
	
	
	format(gText, sizeof(gText), "\wNoClip:\w [ %s%s\w ]", userNoClip[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	format(gText, sizeof(gText), "\wGodMod:\w [ %s%s\w ]", userGodMod[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	format(gText, sizeof(gText), "\wBudowanie:\w [ %s%s\w ]", userAllowBuild[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	format(gText, sizeof(gText), "\wOdpychanie:\w [ %s%s\w ]^n", userPush[id] ? "\r" : "\d", symbolsCustom[SYMBOL_DOT]);
	menu_additem(menu, gText);
	
	menu_additem(menu, "\wPrzywroc Klocki")
	menu_additem(menu, "\wUsun Klocki")
	
	format(gText, sizeof(gText), "\yGod + Odpychanie\d (\w%dsek\d)", timeHelpGod);
	menu_additem(menu, gText);
	menu_additem(menu, "\rZakoncz pomoc!");
	

	menu_setprop(menu, MPROP_EXITNAME, "Wyjscie");
	menu_setprop(menu, MPROP_BACKNAME, "Poprzednie");
	menu_setprop(menu, MPROP_NEXTNAME, "Nastepne");
	
	menu_display(id, menu, userPageHelp[id]/7);
	//show_menu(id, B1 | B2 | B3 | B4 | B5 | B6 | B8 | B9 | B0, gText, -1, "menuPerrmissions"  )
	return PLUGIN_HANDLED;
}
public menuPerrmissions_2(id, menu, item){
	new target = userMoveAs[id];
	
	
	if(!is_user_connected(target)){
		stopHelp(id)
		mainMenuAdmin(id)
		ColorChat(id, GREEN, "---^x01 Gracz ktoremu pomagales wyszedl z serwera!^x04 ---");
		menu_destroy(menu);
		return PLUGIN_CONTINUE;
	}
	
	if(item == MENU_EXIT){
		menu_destroy(menu);
		return PLUGIN_CONTINUE;
	}
	userPageHelp[id] = item;
	
	switch(item){
		case 0:{
			userNoClip[target] =! userNoClip[target]
			set_user_noclip(target, userNoClip[target]);
					
			ColorChat(target, GREEN, "---^x01 Twoj^x04 Noclip^x01 zostal %s przez Admina^x03 %s^x04 ---", userNoClip[target] ? "wlaczony" : "wylaczony" , userName[id]);
			menuPerrmissions(id);
		}
		case 1:{
			userGodMod[target] =! userGodMod[target]
			set_user_godmode(target, userGodMod[target]);
			
			ColorChat(target, GREEN, "---^x01 Twoj^x04 GodMod^x01 zostal %s przez Admina^x03 %s^x04 ---", userGodMod[target] ? "wlaczony" : "wylaczony" , userName[id]);
			menuPerrmissions(id);
		}
		case 2:{
			userAllowBuild[target] =! userAllowBuild[target];
			
			ColorChat(target, GREEN, "---^x01 Twoje^x04 Budowanie^x01 zostalo %s przez Admina^x03 %s^x04 ---", userAllowBuild[target] ? "wlaczone" : "wylaczone" , userName[id]);
			
			stopEnt(target)
			menuPerrmissions(id);
		}
		
		case 3:{
			userNoClip[id] =! userNoClip[id]
			set_user_noclip(id, userNoClip[id]);
			menuPerrmissions(id);
			
		}
		case 4:{
			userGodMod[id] =! userGodMod[id]
			set_user_godmode(id, userGodMod[id]);
			menuPerrmissions(id);
		}
		case 5:{
			userAllowBuild[id] =! userAllowBuild[id];
			menuPerrmissions(id);
		}
		case 6:{
			if(buildTime || prepTime){
				ColorChat(id, GREEN, "---^x01 Funkcja^x03 dostepna^x01 tylko podczas rundy!^x04 ---");
				menuPerrmissions(id)
				return PLUGIN_CONTINUE;
			}
			userPush[id] =! userPush[id]	
			
			if( task_exists(id+TASK_PUSH) && userPush[id] )
				remove_task(id+TASK_PUSH)
			else set_task(0.1, "adminHelpPush", id+TASK_PUSH)
			
			menuPerrmissions(id);
			
		}
		
		case 7:{
			adminResetBlock(1, 0, 0);
			menuPerrmissions(id)
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 przywrocil^x04 klocki^x01 na mapie^x04 ---", userName[id]);
			
			menuPerrmissions(id);
		}
		case 8:{
			adminResetBlock(0, 0, 0);
			menuPerrmissions(id)
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 usunal^x04 klocki^x01 z mapy^x04 ---",  userName[id]);
			
			menuPerrmissions(id);
		}
		case 9:{
			
			if(get_gametime() < userTimeAdmin[target]){
				new Float:got = floatsub(userTimeAdmin[target], get_gametime())
				ColorChat(id, GREEN, "---^x01 Gracz^x03 %s^x01 juz ma to wlaczone^x04 |^x01 Czas:^x03 %0.1f^x04 ---", userName[target], got);
				return PLUGIN_CONTINUE;
				
			}
			
			userGodMod[target] = true;
			set_user_godmode(target, userGodMod[target]);
			
			userPush[target] = true
			
			if( task_exists(target+TASK_PUSH) && userPush[target] )
				remove_task(target+TASK_PUSH)
			else set_task(0.1, "adminHelpPush", target+TASK_PUSH)
			
			ColorChat(id, GREEN, "---^x01 Wlaczyles^x03 GodMod + Opdychanie^x01 na^x03 %dsek^x01 graczowi^x03 %s^x04 ---", timeHelpGod, userName[target]);
			ColorChat(target, GREEN, "---^x01 Admin^x03 %s^x01 wlaczyl Ci GodMod + Odpychanie^x01 na^x03 %d sek!^x04 ---", userName[id], timeHelpGod)
			
			userTimeAdmin[target] = get_gametime() + float(timeHelpGod);
			
			
			set_task(float(timeHelpGod), "offHelp", target);
			
			menuPerrmissions(id);
			
		}
		case 10:{
			
			if(is_user_connected(target)){
				userAllowBuild[target] 	=	false;
				userGodMod[target] 	=	false;
				userNoClip[target] 	=	false;
				
				set_user_godmode(target, false);
				set_user_noclip(target, false);
				
				stopEnt(target)
				
				stopHelp(id)
			
				mainMenuAdmin(id)
				
				ColorChat(target, GREEN, "---^x01 Admin^x03 %s^x01 konczy tobie pomagac^x04 ---", userName[id])
				ColorChat(id, GREEN, "---^x01 Konczysz pomagac graczowi^x03 %s^x04 ---", userName[target])
			}
		} 
		default:{}
	}
	
	if(userNoClip[target] || userGodMod[target] || userAllowBuild[target] || userPush[target]){
		set_rendering(target, kRenderFxGlowShell, accentColorHud[0], accentColorHud[1], accentColorHud[2], kRenderNormal, 5);		
	} else set_rendering(target, kRenderFxGlowShell, 0, 0, 0, kRenderNormal, 0);
	
	return PLUGIN_CONTINUE
}

public stopHelp(id){
	
	userHelpAdmin[id] 	= 	false;
	userMoveAs[id] 		= 	id;
	userPageHelp[id]	=	0;
			
	stopEnt(id)
}

public offHelp(id){
	if( task_exists(id+TASK_PUSH) && userPush[id] ) remove_task(id+TASK_PUSH)
	
	userPush[id] = false;
	userGodMod[id] = false;
	
	set_user_godmode(id, userGodMod[id]);
	
	ColorChat(id, GREEN, "---^x01 Twoj^x03 GodMod + Odpychanie^x01 zostalo wylaczone!^x04 ---")
	
	set_rendering(id, kRenderFxGlowShell, 0, 0, 0, kRenderNormal, 0);
}

public choosePlayer(id, item){
	new gText[128]
	new menu=menu_create("\r[BaseBuilder]\y Wybierz gracza:", "choosePlayer_2")
	
	new daysLeft = 0;	
	new players;
	
	for( new i = 1, x= 0; i < maxPlayers; i ++ ){
		if( !is_user_connected(i) || is_user_hltv(i))
			continue;
		
		switch( userMenuPlayer[id]){
			
			case MENU_VIEW_BLOCK:{
				format(gText, sizeof(gText), "\w%s\d [%s\d]", userName[i], teamNames[get_user_team(i)][1])
			}
			case MENU_CHANGE_TEAM:{
				format(gText, sizeof(gText), "\w%s\d [%s%s\d]", userName[i], (get_user_team(i) == 1) ? "\y" : (get_user_team(i) == 2 ) ? "\r" : "\d", teamNames[get_user_team(i)][1])
			}
			case MENU_PASSWORD:{ 
				format(gText, sizeof(gText), "\y%s -\d [\r%s\d]", userName[i], strlen(userPassword[i])>1?userPassword[i]:"_");
			}
			case MENU_GIVING_VIP:{
				daysLeft = (timeVip[i] - get_systime())
				if(has_flag(i, flagVip))	format(gText, sizeof(gText), "\y%s -\w [Bez Limitu]", userName[i])
				else if( daysLeft <= 0)	format(gText, sizeof(gText), "\d%s - [Brak Vipa]", userName[i])	
				else 			format(gText, sizeof(gText), "\y%s -\d [Pozostalo\r %d:%s%d:%s%d\d]", userName[i],  (daysLeft / HOUR ), ( daysLeft / MINUTE % MINUTE )<10?"0":"", ( daysLeft / MINUTE % MINUTE ), (daysLeft%MINUTE)<10?"0":"", ( daysLeft %MINUTE ));
			} 
			case MENU_GIVING_LUZCOIN:{
				format(gText, sizeof(gText), "\w%s -\d [\r%d Luzaczkow\d]", userName[i], userLuzCoin[i] )
			}
			case MENU_GIVING_NUGGET:{
				format(gText, sizeof(gText), "\w%s -\d [\r%d Brylek\d]", userName[i], userNugget[i] )
			}
			case MENU_GIVING_SKILLPOINT:{
				format(gText, sizeof(gText), "\w%s -\d [\r%d Pkt. Umiej.\d]", userName[i], userClassPoint[i] )
			}
			case MENU_GIVING_LVL:{
				format(gText, sizeof(gText), "\w%s -\d [\r%d Lv.\d]", userName[i], userLevel[i] )
			}
			case MENU_GIVING_TIME:{
				format(gText, sizeof(gText), "\y%s -\d [\r%d:%s%d:%s%d\d]", userName[i],  (playedTime(i) / HOUR ), ( playedTime(i)  / MINUTE % MINUTE )<10?"0":"", ( playedTime(i)  / MINUTE % MINUTE ), (playedTime(i) %MINUTE)<10?"0":"", ( playedTime(i) %MINUTE ));
			}
			case MENU_GIVING_RESET:{
				format(gText, sizeof(gText), "\w%s -\d [\r%d Reset\d]", userName[i], userReset[i] )
			}
			case MENU_GIVING_EXP:{
				format(gText, sizeof(gText), "\w%s", userName[i])
			}
			
			case MENU_PLAYER_NUGGET:{
				if( i == id ) continue;
				format(gText, sizeof(gText), "\w%s", userName[i])
			}
			case MENU_PLAYER_AWARD:{
				new newAwardLeft 	= 	userLastAwardTime[i] + userAwardTime - playedTime(i);
				format(gText, sizeof(gText), "\y%s -\d [\r%d:%s%d:%s%d\d]", userName[i],  (newAwardLeft / HOUR ), ( newAwardLeft  / MINUTE % MINUTE )<10?"0":"", ( newAwardLeft  / MINUTE % MINUTE ), (newAwardLeft %MINUTE)<10?"0":"", (newAwardLeft %MINUTE ));
			}
			
			case MENU_PLAYER_STAMINA:{
				if(!unlockCave[i]) continue;
				new newStaminaLeft  =  userLastStaminaTime[i] + staminaTime + ( userUpgradeMine[i][up_STAMINA] * 720 )  - playedTime(i);
				if(newStaminaLeft <= 0) format(gText, sizeof(gText), "\y%s -\d [\rNie posiada staminy\d]", userName[i]);
				else format(gText, sizeof(gText), "\y%s -\d [\r%d:%s%d:%s%d\d]", userName[i],  (newStaminaLeft / HOUR ), ( newStaminaLeft  / MINUTE % MINUTE )<10?"0":"", ( newStaminaLeft  / MINUTE % MINUTE ), (newStaminaLeft %MINUTE)<10?"0":"", (newStaminaLeft %MINUTE ));
			}
			case MENU_PLAYER_MUTE:{
				
				
				if( i == id ) continue;
				if(isSuperAdmin(i))
					format(gText, sizeof(gText), "\d%s\r*", userName[i])
				else if(userMutePlayer[id][i])
					format(gText, sizeof(gText), "\w%s\y^t%s^t\rWyciszony", userName[i],symbolsCustom[SYMBOL_DR_ARROW])
				else format(gText, sizeof(gText), "\w%s", userName[i])
				
			}
			case MENU_PLAYER_WARNING:{
				if(userWarningAmount[i] == 0) format(gText, sizeof(gText), "\w%s\d |\r Brak Ostrzezen", userName[i])
				else format(gText, sizeof(gText), "\w%s\d |\r %d\w/\r%d", userName[i] ,userWarningAmount[i], MAXWAR)
			}	
			
			case MENU_PLAYER_CAMP:{
				if(!userCheckCamp[i]) continue;
				
				format(gText, sizeof(gText), "%s", userName[i])
			}
			
			case MENU_SELECT_PLAYER:{
				format(gText, sizeof(gText), "%s", userName[i])
			}
			case MENU_GIVING_BONES:{
				format(gText, sizeof(gText), "\w%s -\d [\r%d Kosci\d]", userName[i], userBone[i])
			}
			case MENU_CASE_CREATE:{
				format(gText, sizeof(gText), "%s", userName[i])
			}
		}
		menu_additem(menu, gText)
		userPlayerList[id][x++] = i
		players++;
		
	}
	if (!players) {
		menu_destroy(menu);
		ColorChat(id, GREEN, "---^x01 Na serwerze nie ma gracza!^x04 ---");
	} else menu_display(id, menu, item/7);
}
public choosePlayer_2(id, menu, item){
	if( item == MENU_EXIT ){
		menu_destroy(menu)
		return PLUGIN_HANDLED
	}
	new target = userPlayerList[id][item];
	userService[id] = userPlayerList[id][item]
	
	switch(userMenuPlayer[id]){
		case MENU_VIEW_BLOCK:{
			userMoveAs[id] = target;
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 przenosi klocki jako^x03 %s!^x04 ---", userName[id], userName[target]);
			return PLUGIN_CONTINUE;
		}
		case MENU_CHANGE_TEAM:{
			
			cs_set_user_team(target, get_user_team(target) == 1 ? 2 : 1 )
			respawnPlayerAdmin(target)
			userRoundTeam[target] = get_user_team(target)				
			ColorChat(0, GREEN, "---^x04 %s^x01 przeniesiony do^x04 %s^x01 przez^x04 %s ---", userName[target], teamNames[get_user_team(target)][1], userName[id])
			choosePlayer(id, item)
		
		}
		case MENU_PASSWORD:{
			ColorChat(target, GREEN, "---^x01 Admin^x03 %s^x01 podaje Ci haslo^x04 ---", userName[id])
			ColorChat(target, GREEN, "---^x01 Twoje Haslo:^x03 %s^x04 ---", userPassword[target])
			ColorChat(id, GREEN, "---^x01 Podales haslo:^x03 %s^x01 graczowi:^x03 %s^x04 ---", userPassword[target], userName[target])
		}
		case MENU_GIVING_VIP:{
			cmd_execute(id, "messagemode Ilosc")
		} 
		case MENU_GIVING_LUZCOIN:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_GIVING_NUGGET:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_GIVING_SKILLPOINT:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_GIVING_LVL:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_GIVING_TIME:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_GIVING_RESET:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_GIVING_EXP:{
			cmd_execute(id, "messagemode Ilosc")
		}
		
		case MENU_PLAYER_NUGGET:{
			cmd_execute(id, "messagemode Ilosc_Brylek")
		}
		case MENU_PLAYER_AWARD:{
			new gText[128];
			logType[id] = LOG_ADD;
			if(logType[id] == LOG_ADD){
				userLastAwardTime[target]		=	playedTime(target)
				ColorChat(id, 		GREEN, 	"%s Zresetowales nagrode gracza^x04 %s!", PREFIXSAY, userName[target] ) 
				ColorChat(target, 	GREEN, 	"%s Twoja nagroda zostala zresetowana przez Admina^x04 %s!", PREFIXSAY, userName[id] ) 
				format(gText, sizeof(gText), "zresetowal nagrode [%s]", userName[target])
				logBB(id, gText)
			}
		}
		case MENU_PLAYER_STAMINA:{
			if(!unlockCave[target]) {
				choosePlayer(id, target)
				return PLUGIN_CONTINUE;
			}
			new gText[128];
			logType[id] = LOG_ADD;
			if(logType[id] == LOG_ADD){
				userLastStaminaTime[target] 	= playedTime(target)
				ColorChat(id, 		GREEN, 	"%s Zresetowales wyczerpanie gracza^x04 %s!", PREFIXSAY, userName[target] ) 
				ColorChat(target, 	GREEN, 	"%s Twoje wyczerpanie zostala zresetowana przez Admina^x04 %s!", PREFIXSAY, userName[id] ) 
				format(gText, sizeof(gText), "zresetowal wyczerpanie [%s]", userName[target])
				logBB(id, gText)
			}
		}
		case MENU_PLAYER_MUTE:{
			
			if(isSuperAdmin(target)){
				ColorChat(id, GREEN, "%s Nie mozesz wyciszyc opiekuna!", PREFIXSAY);
				choosePlayer(id, 0)
				return PLUGIN_CONTINUE;
			}
			
			userMutePlayer[id][target] =! userMutePlayer[id][target]
			ColorChat(id, GREEN, "%s %s sobie gracza:^x03 %s", PREFIXSAY, userMutePlayer[id][target] ? "Wyciszyles" : "Odciszyles", userName[target]);
			
		}
		case MENU_PLAYER_WARNING:{
		
			userWarningInfo[id] = target;
			warningInfo(id)
			return PLUGIN_CONTINUE;
			
		}
		case MENU_PLAYER_CAMP:{
			adminResetBlock(4, id, target)
			userCheckCamp[target] = false;
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 przywrocil sobie kampe gracza:^x03 %s^x04 ---", userName[id], userName[target]);
		}
		case MENU_SELECT_PLAYER:{
			viewBlock[id] = target;
			mainMenuAdmin(id)
			return PLUGIN_CONTINUE;
		}
		case MENU_GIVING_BONES:{
			cmd_execute(id, "messagemode Ilosc")
		}
		case MENU_CASE_CREATE:{
			caseCreate[id][CREATE_CASE_PLAYER] = target;
			menuCreateCase(id)
			return PLUGIN_CONTINUE;
		}
	}
	choosePlayer(id, 0)
	return PLUGIN_HANDLED
}
public reWriteBlocksParty(id, team){
	adminResetBlock(3, team, id)
}	

new Float:saveEntOrigin[1024][3];

public adminResetBlock(type, id1, id2){
	new szClass[10], szTarget[7];
	new Float:fOrigin[3]	
	for(new ent=maxPlayers+1; ent<maxEnts;ent ++){
		if( !pev_valid(ent) )
			continue;
		
		if( ent == gBarrier ) 
			continue;
			
		entity_get_string(ent, EV_SZ_classname, szClass, 9);
		entity_get_string(ent, EV_SZ_targetname, szTarget, 6);
		
		if( !equal(szClass, "func_wall") || containi(szTarget, "ignore") !=-1 || containi(szTarget, "JUMP")!=-1 )
			continue;
		if( type == 0 ){			
			if( getOwner(ent) != 0 )
				continue 
			engfunc( EngFunc_SetOrigin, ent, Float:{ -8192.0, -8192.0, -8192.0 } );
			unSetBlock( ent )
		}else if( type == 1 ){			
			if( getOwner(ent) != 0 )
				continue 	
			entity_get_vector(ent, EV_VEC_vuser3, fOrigin )
			engfunc( EngFunc_SetOrigin, ent, fOrigin );
			unSetBlock( ent )
		}
		else if( type == 2 ){			
			if( getOwner(ent) != id1 && getOwner(ent) != id2 )
				continue 
			
			entity_get_vector(ent, EV_VEC_origin, fOrigin )
			
			userCheckCamp[id2] = true;
			
			for(new i = 0; i < 3; i ++) saveEntOrigin[ent][i] = fOrigin[i]
				
			engfunc( EngFunc_SetOrigin, ent, Float:{ -8192.0, -8192.0, -8192.0 } );
			//unSetBlock( ent )
		}
		else if( type == 3 ){			
			if( getOwner(ent) != id2  )
				continue 
				
			setOwner(ent, id1)
		}
		else if( type == 4){
			
			
			if( getOwner(ent) != id2 )
				continue 
				
			engfunc( EngFunc_SetOrigin, ent, saveEntOrigin[ent] );	
				
			setOwner(ent, id1)
			setLastMover(ent, id1)	
		}
	}	
}

public adminCommands(id, szMessage[]){
	if(isSuperAdmin(id)){			
		if( equal(szMessage, "/swiatlo")){
			light(id)
			return PLUGIN_HANDLED
		}
		if( equal(szMessage, "/clone", 6)){
			adminLockBlock(id)
			return PLUGIN_HANDLED
		}
		if(equal(szMessage, "/runhappy")){
			happyMenu(id)
			return PLUGIN_HANDLED
		}
		if(equal(szMessage, "/hp", 3)){	
			new szName[33], szCommand[10], szValue[11]
			parse( szMessage,
				szCommand, 	sizeof(szCommand),
				szName, 	sizeof(szName),
				szValue, sizeof(szValue)
			)
			new target = cmd_target(id, szName, 0)
			new gValue = str_to_num(szValue);
				
			if(!target){
				ColorChat(id, GREEN, "%s Nie znaleziono takiego gracza!", PREFIXSAY);
				return PLUGIN_HANDLED
			}
			if(!gValue){
				ColorChat(id, GREEN, "%s Wpisz ile chcesz dodac!", PREFIXSAY);
				return PLUGIN_HANDLED
			}
			set_user_health(target, get_user_health(target) + gValue);			
			ColorChat(0, GREEN, "---^x01 Gracz^x03 %s^x01 dostal^x04 %dHP^x01 od^x03 %s^x04 ---", userName[target], gValue, userName[id])
			return PLUGIN_HANDLED
		}
		if( 0 <= contain(szMessage, "/tp")){
			new szName[33], szCommand[10]
			parse( szMessage,
				szCommand, 	sizeof(szCommand),
				szName, 	sizeof(szName)
			)
			new target = cmd_target(id, szName, 0)
			if(!target){
				ColorChat(id, GREEN, "%s Nie znaleziono takiego gracza!", PREFIXSAY);
				userVarMenu[id] = 1 ;
				menuSpecifyUser(id, szName)
				return PLUGIN_HANDLED
			}
			userNoClip[id] = true;
			set_user_noclip(id, userNoClip[id]);
			mainMenuAdmin(id)
			new Float:fOrigin[3] = 0.0;
			pev(target, pev_origin, fOrigin);
			set_pev(id, pev_origin, fOrigin);
			return PLUGIN_HANDLED;
		}
		if( 0 <= contain(szMessage, "/tome")){
			new szName[33], szCommand[10]
			parse( szMessage,
				szCommand, 	sizeof(szCommand),
				szName, 		sizeof(szName)
			)
			
			if(equal(szName, "#all")){
				for(new i = 1; i < maxPlayers; i++){
					if(!is_user_connected(i) || !is_user_alive(i))
						continue; 
					new Float:fOrigin[3] = 0.0;
					pev(id, pev_origin, fOrigin);
					set_pev(i, pev_origin, fOrigin);	
					
				}
				return PLUGIN_CONTINUE;
			} else if(equal(szName, "#tt")){
				for(new i = 1; i < maxPlayers; i++){
					if(!is_user_connected(i) || !is_user_alive(i))
						continue; 
					
					if(get_user_team(i) != 1)
						continue; 
						
					new Float:fOrigin[3] = 0.0;
					pev(id, pev_origin, fOrigin);
					fOrigin[2] += 50.0;
					set_pev(i, pev_origin, fOrigin);	
						
				}
				return PLUGIN_CONTINUE;
			} else if(equal(szName, "#ct")){
				for(new i = 1; i < maxPlayers; i++){
					if(!is_user_connected(i) || !is_user_alive(i))
						continue; 
						
					if(get_user_team(i) != 2)
						continue; 
							
					new Float:fOrigin[3] = 0.0;
					pev(id, pev_origin, fOrigin);
					set_pev(i, pev_origin, fOrigin);		
				}
				return PLUGIN_CONTINUE;
			} 
			
			
			new target = cmd_target(id, szName, 0)
			if(!target){
				ColorChat(id, GREEN, "%s Nie znaleziono takiego gracza!", PREFIXSAY);
				userVarMenu[id] = 2;
				menuSpecifyUser(id, szName)
				return PLUGIN_HANDLED
			}
			
			if(target){
				userNoClip[id] = true
				set_user_noclip(id, userNoClip[id]);
				mainMenuAdmin(id)
				new Float:fOrigin[3] = 0.0;
				pev(id, pev_origin, fOrigin);
				set_pev(target, pev_origin, fOrigin);
			}
			
			
			return PLUGIN_HANDLED;
		}
	}
	if(equal(szMessage, "/warn")||equal(szMessage, "/ostrzez", 8)){	
		userMenuPlayer[id] = MENU_PLAYER_WARNING
		choosePlayer(id, 0)
		
		return PLUGIN_HANDLED
	}
	
	if(equal(szMessage, "/a")||equal(szMessage, "/admin")){	
		if(userHelpAdmin[id])
			menuPerrmissions(id)
		else mainMenuAdmin(id)
		
		return PLUGIN_HANDLED
	}
	if(equal(szMessage, "/release")){	
		Release_Zombies()	
		if( buildTime || prepTime ){
			Release_Zombies()	
			ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 rozpoczal runde przed czasem^x04  ---", userName[id])
		}else ColorChat(id, GREEN, "---^x01 Runda juz zostala rozpoczeta^x04  ---")
		return PLUGIN_HANDLED
	}
	if(equal(szMessage, "/revive", 7)){	
		new szName[33], szCommand[10]
		parse( szMessage,
			szCommand, 	sizeof(szCommand),
			szName, 	sizeof(szName)
		)
		new target = cmd_target(id, szName, 0)
		if( !target ) {
			userVarMenu[id] = 0;
			menuSpecifyUser(id, szName)
			return PLUGIN_HANDLED
		}
		
		respawnPlayerAdmin(target)
		ColorChat(0, GREEN, "---^x01 Gracz^x03 %s^x01 zostal ozywiony przez^x03 %s^x04 ---", userName[target], userName[id])
		return PLUGIN_HANDLED
	}
	if(equal(szMessage, "/swap", 5)){	
		new szName[33], szCommand[10]
		parse( szMessage,
			szCommand, 	sizeof(szCommand),
			szName, 	sizeof(szName)
		)
		new target = cmd_target(id, szName, 0)
		if( !target ) {
			userVarMenu[id] = 0;
			menuSpecifyUser(id, szName)
			return PLUGIN_HANDLED
		}
		
		cs_set_user_team(target, get_user_team(target) == 1 ? 2 : 1 )
		respawnPlayerAdmin(target)
		userRoundTeam[target] = get_user_team(target)				
		ColorChat(0, GREEN, "---^x01 Gracz^x03 %s^x01 zostal przeniesiony do^x04 %s^x01 przez^x03 %s^x04 ---", userName[target], teamNames[get_user_team(target)][1], userName[id])
		return PLUGIN_HANDLED
	}
	if(equal(szMessage, "/mute", 5)){	
		new szName[33], szCommand[10], szValue[11]
		parse( szMessage,
			szCommand, 	sizeof(szCommand),
			szName, 	sizeof(szName),
			szValue, sizeof(szValue)
		)
		new target = cmd_target(id, szName, 0)
		new gValue = str_to_num(szValue);
			
		if(!target){
			ColorChat(id, GREEN, "%s Nie znaleziono takiego gracza!", PREFIXSAY);
			return PLUGIN_CONTINUE
		}
		if(gValue){
			
			new gText[128];
			logType[id] = LOG_MUTE;
			if(logType[id] == LOG_MUTE) format(gText, sizeof(gText), "zostal zmutowany na [%d min] przez [%s]", gValue, userName[target])
			logBB(id, gText)
			
			userMute[target]	= 	max( userMute[target] + (MINUTE*gValue), get_systime() + (MINUTE*gValue) )
			
			ColorChat(target, GREEN, "%s Zostales^x04 wyciszony^x01 na^x04 %d^x01 minut! [Admin:^x04 %s^x01]", PREFIXSAY, gValue, userName[id]);
			ColorChat(id, GREEN, "%s Gracz^x04 %s^x01 zostal^x04 wyciszony^x01 na^x04 %d^x01 minut", PREFIXSAY, userName[target], gValue);
		} else {
			
			new gText[128];
			logType[id] = LOG_MUTE;
			if(logType[id] == LOG_MUTE) format(gText, sizeof(gText), "zostal odciszony przez [%s]", userName[target])
			logBB(id, gText)
			
			ColorChat(target, GREEN, "%s Zostales^x04 odciszony^x01! [Admin:^x04 %s^x01]", PREFIXSAY, userName[id]);
			ColorChat(id, GREEN, "%s Gracz^x04 %s^x01 zostal^x04 odciszony", PREFIXSAY, userName[target]);
			userMute[target] = 0;
		}
		return PLUGIN_HANDLED
	}
	return PLUGIN_CONTINUE
}
public menuSpecifyUser(id, szName[]){
	new gText[128], szNameEdit[48]
	new menu = menu_create("\r[BaseBuilder]\y O kogo Ci chodzi?", "menuSpecifyUser_2")
	strToLower(szName)
	for( new i = 1, x = 0;  i < maxPlayers; i ++ ){
		if( !is_user_connected(i) || is_user_hltv(i))
			continue
			
			
		if( containi(userName[i], szName) == -1)
			continue
		
		format(szNameEdit, sizeof(szNameEdit), "\r%s\w", szName)		
		strToLower(szName)
		format(gText, sizeof(gText), "%s", userName[i])
		strToLower(gText)
		replace_all(gText, sizeof(gText)-1, szName, szNameEdit)
		strFix(gText, userName[i])
		menu_additem(menu, gText)
		userVarList[id][x++] = i 
	}
	
	
	menu_display(id, menu, 0)
}
public strFix(szString[], szName[]){
	for( new i = 0, x = 0 ; i < strlen(szString); i ++){
		if(szString[i] == 92){
			i++
			continue
		}
		szString[i] = szName[x++]
	}
}

public strToUpper( szString[]){
	for( new i = 0 ; i < strlen(szString) ; i ++ ){		
		szString[i] = toupper(szString[i])
	}
}
public strToLower( szString[]){
	for( new i = 0 ; i < strlen(szString) ; i ++ ){
		szString[i] = tolower(szString[i])
	}
}
public menuSpecifyUser_2(id, menu, item){
	if( item == MENU_EXIT ){
		menu_destroy(menu)
		return
	}
	new target = userVarList[id][item]
	switch(userVarMenu[id]){
		case 0:{
			respawnPlayerAdmin(target)
			ColorChat(0, GREEN, "---^x01 Gracz^x03 %s^x01 zostal ozywiony przez^x03 %s^x04 ---", userName[target], userName[id])
		}
		case 1:{
			userNoClip[id] = true
			set_user_noclip(id, userNoClip[id]);
			mainMenuAdmin(id)
			new Float:fOrigin[3] = 0.0;
			pev(target, pev_origin, fOrigin);
			set_pev(id, pev_origin, fOrigin);
		}
		case 2:{
			userNoClip[id] = true
			set_user_noclip(id, userNoClip[id]);
			mainMenuAdmin(id)
			new Float:fOrigin[3] = 0.0;
			pev(id, pev_origin, fOrigin);
			set_pev(target, pev_origin, fOrigin);
		}
	}
}



public adminHelp(id){
	if (!prepTime){
		ColorChat(id, GREEN, "%s Poczekaj na czas przygotowania", PREFIXSAY);
		return 1;
	}
	if (gameTime){
		ColorChat(id, GREEN, "%s Juz minal czas na pomoc!", PREFIXSAY);
		return 1;
	}
	if (get_user_team(id) != 2){
		ColorChat(id, GREEN, "%s Musisz byc w budowniczych", PREFIXSAY);
		return 1;
	}
	
	if(userMaxHelp[id] <= 0){
		ColorChat(id, GREEN, "%s Juz nie mozesz wiecej razy wezwac POMOC", PREFIXSAY)
		return 1;
	}
	if(userHelp[id]){
		ColorChat(id, GREEN, "%s Prosiles juz o pomoc!", PREFIXSAY);
		return 1;
	}
	new space = firstFree();
	
	if(space == -1){
		ColorChat(id, GREEN, "%s Za duzo osob prosilo o pomoc", PREFIXSAY, userName[id]);
		return 1;
		
	}
	userHelp[id] 		= 	true;
	needHelp[space] 		= 	id 
	ColorChat(id, GREEN, "%s Jeszcze tylko^x04 %d^x01 razy mozesz prosic dzis o pomoc", PREFIXSAY, userMaxHelp[id]);
	ColorChat(0, GREEN, "%s Gracz^x03 %s^x01 prosi o pomoc!", PREFIXSAY, userName[id]);
	return 0;
}
public bool:isInHelp(id){
	
	for( new i = 0 ; i< sizeof( needHelp ) ; i ++ ){
		if( needHelp[i] == 0 )
			break;
		
		if( needHelp[i] == id)
			return true;
	}
	return false;
}
public firstFree(){
	for( new i = 0 ; i< sizeof( needHelp ) ; i ++ ){
		if( needHelp[i] != 0 )
			continue
		
		return i;
	}
	return -1;
}	
public findHelp(index){
	for( new i = 0 ; i< sizeof( needHelp ) ; i ++ ){
		if( needHelp[i] != index )
			continue
		
		return i;
	}
	return 1;	
}
public menuAdminHelp(id){
	new menu = menu_create("Kto prosil:", "menuAdminHelp_2")
	new target =  0;
	for( new i = 0, x = 0; i < sizeof( needHelp ); i++ ){
		if( needHelp[i] == 0 )
			break
		if( needHelp[i] == -1 )
			continue
			
		target = needHelp[i]
		menu_additem(menu, userName[target])
		menuList[id][x++] = target
			
	}
	menu_display(id, menu, 0)
	
	return target;
}
public menuAdminHelp_2(id,menu,item){
	if( item == MENU_EXIT ){
		menu_destroy(menu)
		return
	}
	
	new target 	= 	menuList[id][item]
	new in 		= 	findHelp(target)
	
	new Float:fOrigin[3]
	pev(target, 	pev_origin, 	fOrigin)
	set_pev(id, 	pev_origin, 	fOrigin)
		
	needHelp[in]		=	-1
	userHelpPoint[id]	++;
	userMaxHelp[target] 	-=	1
	userGodMod[target]	=	true	
	userAllowBuild[id] 	= 	true	
	userNoClip[id]		=	true
	userGodMod[id]		=	true	
	userVarMenu[id] 		= 	target;
	userMoveAs[id] 		= 	target
	userHelpAdmin[id] 	= 	true
	set_user_godmode(target, userGodMod[id])
	set_user_noclip(id, userNoClip[id])
	set_user_godmode(id, userGodMod[id])
		
	ColorChat(0, GREEN, "---^x01 Admin^x03 %s^x01 pomaga graczowi^x03 %s^x04 ---", userName[id], userName[target])
	menuPerrmissions(id)	
	
	return
}
public light(id){
	new gText[256], bar[256];
	if(!lightType[0] ) format(gText, sizeof(gText),"\dSwiatlo:\r Zwyczajne");
	else format(gText, sizeof(gText),"\dSwiatlo:\r %d%%\w [\d %d/%d\w -\d %c\w ]",  ( lightType[0] * 100 / strlen(lightCharacter)), lightType[0], strlen(lightCharacter), lightCharacter[lightType[0]-1]);
	new menu = menu_create(gText, "light_2");
	barMenu(bar, sizeof(bar), lightType[0] , strlen(lightCharacter), "|", "|") ;   

	format(gText, sizeof(gText), "%s", bar);
	menu_additem(menu, bar);

		
	menu_display(id, menu, 0);
}	
public light_2(id, menu, item){
	if(item == MENU_EXIT){
		menu_destroy(menu);
		return;
	}
	switch(item){
		case 0:{
			new bufferBit[32];
			
			if((lightType[0] ++) >= strlen(lightCharacter)){
				for(new i = 0; i < 2; i ++) lightType[i] = 0;
				set_lights("#OFF");
			}	
			lightType[1] ^= ( 1 << lightType[0] - 1 );
			stringBuffer(lightType[1], bufferBit, sizeof(bufferBit));
			set_lights(bufferBit);
			light(id);
		}	
		
	}
}
stock stringBuffer(flags, buffer[], size) {
	format(buffer, size, "");
	for (new i = 0; i < sizeof(lightCharacter); i++)
	if (flags & (1 << i)) format(buffer, size, "%c", lightCharacter[i]);
}
stock barMenu(gText[], iLen, type, amount, const symbolOne[], const symbolTwo[]){
	new line = 0;
	for(new i = 0; i < type; i++) line += format(gText[line], iLen - line - 1, "\y%s\d", symbolOne);
	for(new i = 0; i < amount-type; i++)  line += format(gText[line], iLen - line - 1, "%s", symbolTwo);        
} 
public  selectAllBlockMenu(id){
	
	new menu = menu_create(formatm("\r[BaseBuilder]\y Boki Gracza:\w %s", userName[viewBlock[id]]), "selectAllBlockMenu_2");
	
	menu_additem(menu, formatm("\yCzy bloki sa podswietlone:\d[ %s\d ]^n", selectBlock[id] ? "\rTak" : "\wNie"));
	menu_additem(menu, "Zmien gracza");
	menu_additem(menu, "Usun bloki^n");
	
	menu_additem(menu, formatm("%s++ \w -%s --^n", typeMoveBlock[id] ? "\d" : "\r", typeMoveBlock[id] ? "\r" : "\d"));
	
	if(typeMoveBlock[id]){
		menu_additem(menu, "Przesuwanie\y X-")
		menu_additem(menu, "Przesuwanie\y Y-")
		menu_additem(menu, "Przesuwanie\y Dol")
	} else {
		menu_additem(menu, "Przesuwanie\r X+")
		menu_additem(menu, "Przesuwanie\r Y+")
		menu_additem(menu, "Przesuwanie\r Gora")
	}
	menu_display(id, menu, 0);
}
public selectAllBlockMenu_2(id, menu, item){
	if(item == MENU_EXIT){
		menu_destroy(menu);
		return;
	}

	switch(item){
		case 0:{
			selectBlock[id] =! selectBlock[id]
			selectAllBlock(id);
		}
		case 1:{
			new ent; 
			get_user_aiming(id, ent)
		
			if(getOwner(ent) == 0 || !canBeMoved(id, ent)){
				ColorChat(id, GREEN, "%s Nie mozesz tego zaznaczyc!", PREFIXSAY);
				selectAllBlockMenu(id)
				return;	
			}
			selectBlock[id] = false;
			selectAllBlock(id);
			viewBlock[id] = getOwner(ent);
			selectAllBlockMenu(id)
		}
		case 2:{
			if(selectBlock[id]){
				selectBlock[id] = false;
				adminResetBlock(2, viewBlock[id], viewBlock[id])
			} else ColorChat(id, GREEN, "%s Zaznacz najpierw a potem usuwaj!", PREFIXSAY);
		}
		case 3:{
			typeMoveBlock[id] =! typeMoveBlock[id];
			selectAllBlockMenu(id)
		}
		case 4..6:{
			new szClass[10], szTarget[10];
			new Float:fOrigin[3];
			for(new ent = maxPlayers; ent < 1024; ent  ++){
								
				if( !pev_valid(ent) )
					continue;	
				
				entity_get_string(ent, EV_SZ_classname, szClass, 9);
				entity_get_string(ent, EV_SZ_targetname, szTarget, 9);
								
				if( !equal(szClass, "func_wall") || containi(szTarget, "ignore")!=-1 || containi(szTarget, "JUMP")!=-1)
					continue;
					
				if(getOwner(ent) != viewBlock[id])
					continue;
				
				
				entity_get_vector(ent, EV_VEC_origin, fOrigin)
				pev(ent, pev_origin, fOrigin);
						
				if(item == 4){
					if(typeMoveBlock[id]) fOrigin[0] -= 10.0;
					else fOrigin[0] += 10.0;
				}
				if(item == 5){
					if(typeMoveBlock[id]) fOrigin[1] -= 10.0;
					else fOrigin[1] += 10.0;
				}
				if(item == 6){
					if(typeMoveBlock[id]) fOrigin[2] -= 10.0;
					else fOrigin[2] += 10.0;
				}
				entity_set_vector(ent, EV_VEC_origin, fOrigin)				
			}
			selectAllBlockMenu(id)
		}
		
	}
	selectAllBlockMenu(id)	
}	
/* AMXX-Studio Notes - DO NOT MODIFY BELOW HERE
*{\\ rtf1\\ ansi\\ deff0{\\ fonttbl{\\ f0\\ fnil Tahoma;}}\n\\ viewkind4\\ uc1\\ pard\\ lang1045\\ f0\\ fs16 \n\\ par }
*/
