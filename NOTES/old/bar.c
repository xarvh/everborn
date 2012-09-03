

 a = city_prod_cost(scity, tq);
 x = PROD_BAR_X;
 if(a) x += (scity->prod_status*PROD_BAR_W)/a - PROD_BAR_W;
 draw16(menu[MENU_PRODBAR], x, PROD_BAR_Y);

