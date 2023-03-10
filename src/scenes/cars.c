/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "scenes/scenes.h"

void SetCarColors(ubyte ndex)
{
    // 2018-09-26 LucyG: finally fixed
    const ubyte Col[10 * 12] = {8,  8,  8,  7,  7,  7,  6,  6,  6,  5,  5, 5,  1,  12, 8,  3,  9,  6, 3,  6,
                                4,  2,  5,  3,  7,  15, 15, 7,  13, 12, 5, 10, 10, 4,  8,  8,  12, 0, 7,  10,
                                0,  9,  9,  0,  8,  7,  0,  6,  12, 8,  7, 11, 7,  6,  9,  6,  6,  8, 5,  5,
                                13, 13, 13, 12, 12, 12, 10, 10, 10, 8,  8, 8,  14, 0,  3,  13, 0,  3, 11, 0,
                                2,  9,  0,  2,  0,  14, 1,  0,  12, 0,  0, 10, 0,  0,  8,  0,  0,  6, 15, 0,
                                4,  13, 0,  2,  11, 0,  0,  9,  15, 15, 6, 13, 13, 3,  11, 11, 1,  9, 9,  0};

    ndex *= 12;

    // RGB8
    gfxSetRGB(l_wrp, 8, Col[ndex] << 4, Col[ndex + 1] << 4, Col[ndex + 2] << 4);
    gfxSetRGB(l_wrp, 9, Col[ndex + 3] << 4, Col[ndex + 4] << 4, Col[ndex + 5] << 4);
    gfxSetRGB(l_wrp, 10, Col[ndex + 6] << 4, Col[ndex + 7] << 4, Col[ndex + 8] << 4);
    gfxSetRGB(l_wrp, 11, Col[ndex + 9] << 4, Col[ndex + 10] << 4, Col[ndex + 11] << 4);

    gfxSetRGB(l_wrp, 40, Col[ndex] << 3, Col[ndex + 1] << 3, Col[ndex + 2] << 3);
    gfxSetRGB(l_wrp, 41, Col[ndex + 3] << 3, Col[ndex + 4] << 3, Col[ndex + 5] << 3);
    gfxSetRGB(l_wrp, 42, Col[ndex + 6] << 3, Col[ndex + 7] << 3, Col[ndex + 8] << 3);
    gfxSetRGB(l_wrp, 43, Col[ndex + 9] << 3, Col[ndex + 10] << 3, Col[ndex + 11] << 3);
}

char *tcShowPriceOfCar(uint32_t nr, uint32_t type, void *data)
{
    static char line[TXT_KEY_LENGTH];
    char line1[TXT_KEY_LENGTH];
    Car car = (Car)data;

    txtGetFirstLine(BUSINESS_TXT, "PRICE_AND_MONEY", line1);
    sprintf(line, line1, tcGetCarPrice(car));

    return line;
}

void tcBuyCar(void)
{
    LIST *bubble = NULL;
    ubyte choice = 0;
    ubyte choice1 = 0;
    Car matts_car = NULL;
    Person marc = (Person)dbGetObject(Person_Marc_Smith);

    while ((choice1 != 2) && (choice != GET_OUT))
    {
        ObjectListSuccString = (char *(*)(uint32_t, uint32_t, void *))tcShowPriceOfCar;
        ObjectListWidth = 48;

        hasAll(Person_Marc_Smith,
               OLF_ALIGNED | OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR | OLF_ADD_SUCC_STRING, Object_Car);
        bubble = ObjectListPrivate;

        ObjectListSuccString = NULL;
        ObjectListWidth = 0;

        if (!(LIST_EMPTY(bubble)))
        {
            char exp[TXT_KEY_LENGTH];

            txtGetFirstLine(BUSINESS_TXT, "THANKS", exp);
            ExpandObjectList(bubble, exp);

            ShowMenuBackground();

            if (ChoiceOk(choice = Bubble(bubble, 0, 0L, 0L), GET_OUT, bubble))
            {
                matts_car = (Car)dbGetObject(((struct ObjectNode *)GetNthNode(bubble, (uint32_t)choice))->nr);

                SetCarColors((ubyte)matts_car->ColorIndex);
                gfxShow((uword)matts_car->PictID, GFX_NO_REFRESH | GFX_OVERLAY, 0L, -1L, -1L);

                if (Present(((struct ObjectNode *)GetNthNode(bubble, (uint32_t)choice))->nr, "Car", InitCarPresent))
                {
                    choice1 = Say(BUSINESS_TXT, 0, MATT_PICTID, "AUTOKAUF");

                    AddVTime(7);

                    if (choice1 == 1)
                    {
                        int32_t price = tcGetCarPrice(matts_car);

                        if (tcSpendMoney(price, 0))
                        {
                            uint32_t carID = ((struct ObjectNode *)GetNthNode(bubble, (uint32_t)choice))->nr;

                            hasSet(Person_Matt_Stuvysunt, carID);
                            hasUnSet(Person_Marc_Smith, carID);

                            Say(BUSINESS_TXT, 0, marc->PictID, "GOOD CAR");
                        }

                        if (Say(BUSINESS_TXT, 0, MATT_PICTID, "NACH_AUTOKAUF") == 1) choice1 = 2;
                    }
                }

                gfxShow(27, GFX_NO_REFRESH | GFX_ONE_STEP, 0L, -1L, -1L);
            }
            else
                choice = GET_OUT;
        }
        else
        {
            Say(BUSINESS_TXT, 0, marc->PictID, "NO_CAR");
            choice = GET_OUT;
        }

        RemoveList(bubble);
    }
}

void tcCarInGarage(uint32_t carID)
{
    ubyte choice1 = 0;
    Car matts_car = NULL;
    Person marc = (Person)dbGetObject(Person_Marc_Smith);

    matts_car = (Car)dbGetObject(carID);

    while (choice1 != 5)
    {
        ShowMenuBackground();

        switch (choice1 = Say(BUSINESS_TXT, choice1, 7, "GARAGE"))
        {
            case 0:
                /* Analyse des Wagens */
                tcCarGeneralOverhoul(matts_car);
                break;
            case 1:
                /* Karosserie */
                tcRepairCar(matts_car, "BodyRepair");
                break;
            case 2:
                /* Reifen */
                tcRepairCar(matts_car, "TyreRepair");
                break;
            case 3:
                /* Motor  */
                tcRepairCar(matts_car, "MotorRepair");
                break;
            case 4:
                if (carID != Car_Jaguar_XK_1950)
                    tcColorCar(matts_car);
                else
                    Say(BUSINESS_TXT, 0, marc->PictID, "JAGUAR_COLOR");
                break;
            default:
                break;
        }
    }
}

void tcColorCar(Car car)
{
    LIST *colors = NULL;
    LIST *bubble = NULL;
    ubyte choice = 0;
    uint32_t costs = 0;
    Person marc = (Person)dbGetObject(Person_Marc_Smith);

    costs = (uint32_t)tcColorCosts(car);

    bubble = txtGoKeyAndInsert(BUSINESS_TXT, "LACKIEREN", costs, NULL);

    SetPictID(marc->PictID);
    Bubble(bubble, 0, 0L, 0L);
    RemoveList(bubble);

    if (Say(BUSINESS_TXT, 0, MATT_PICTID, "LACKIEREN_ANT") == 0)
    {
        colors = txtGoKey(OBJECTS_ENUM_TXT, "enum_ColorE");

        txtPutCharacter(colors, 0, '*');

        if (tcSpendMoney(costs, 1))
        {
            char exp[TXT_KEY_LENGTH];

            txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
            ExpandObjectList(colors, exp);

            if (ChoiceOk(choice = Bubble(colors, (ubyte)car->ColorIndex, 0L, 0L), GET_OUT, colors))
            {
                car->ColorIndex = choice;

                SetCarColors(car->ColorIndex);
                gfxPrepareRefresh();
                PlayAnim("Umlackieren", 3000, GFX_DONT_SHOW_FIRST_PIC);

                inpSetWaitTicks(180);  // 200

                inpWaitFor(INP_LBUTTONP | INP_TIME);

                StopAnim();
                inpSetWaitTicks(1L);

                gfxRefresh();
                // gfxShow(26,GFX_NO_REFRESH|GFX_ONE_STEP,0L,-1L,-1L);
                // gfxShow((uword)car->PictID,GFX_NO_REFRESH|GFX_OVERLAY,1L,-1L,-1L);
            }
        }

        RemoveList(colors);
    }

    AddVTime(137);
}

void tcSellCar(uint32_t ObjectID)
{
    LIST *bubble = NULL;
    uint32_t offer = 0;
    Car car = NULL;
    Person marc = (Person)dbGetObject(Person_Marc_Smith);

    car = (Car)dbGetObject(ObjectID);
    offer = tcGetCarTraderOffer(car);

    if (tcRGetCarAge(car) < 1)
        bubble = txtGoKeyAndInsert(BUSINESS_TXT, "ANGEBOT_1", tcRGetCarValue(car), offer, NULL);
    else
        bubble = txtGoKeyAndInsert(BUSINESS_TXT, "ANGEBOT", tcRGetCarValue(car), tcRGetCarAge(car), offer, NULL);

    SetPictID(marc->PictID);
    Bubble(bubble, 0, 0L, 0L);
    RemoveList(bubble);

    if ((Say(BUSINESS_TXT, 0, MATT_PICTID, "VERKAUF")) == 0)
    {
        if (GamePlayMode & GP_MORE_MONEY)
        {  // Lucy 2017-11-08 new cheat
            if (offer > 0) offer = (offer * 3) / 2;
        }
        tcAddPlayerMoney(offer);

        hasSet(Person_Marc_Smith, ObjectID);
        hasUnSet(Person_Matt_Stuvysunt, ObjectID);
    }

    gfxShow(27, GFX_NO_REFRESH | GFX_ONE_STEP, 0L, -1L, -1L);
    AddVTime(97);
}

void tcRepairCar(Car car, char *repairWhat)
{
    LIST *presentationData = (LIST *)CreateList(0);
    LIST *list = NULL;
    ubyte *item = NULL;
    ubyte enough = 1;
    ubyte type = 7;
    ubyte ready = 0;
    uint32_t costs = 0L;
    uint32_t choice = 0L;
    uint32_t totalCosts = 0L;
    uword line = 0;
    Person marc = (Person)dbGetObject(Person_Marc_Smith);

    if (strcmp(repairWhat, "MotorRepair") == 0)
    {
        item = &car->MotorState;
        costs = tcCostsPerEngineRepair(car);
        type = 1;
    }
    else
    {
        if (strcmp(repairWhat, "BodyRepair") == 0)
        {
            item = &car->BodyWorkState;
            costs = tcCostsPerBodyRepair(car);
            type = 2;
        }
        else
        {
            if (strcmp(repairWhat, "TyreRepair") == 0)
            {
                item = &car->TyreState;
                costs = tcCostsPerTyreRepair(car);
                type = 4;
            }
            else
                costs = tcCostsPerTotalRepair(car);
        }
    }

    if (!(enough = tcSpendMoney(costs, 0))) return;

    list = txtGoKey(PRESENT_TXT, repairWhat);

    gfxPrepareRefresh();
    gfxShow(BIG_SHEET, GFX_NO_REFRESH | GFX_OVERLAY, 0L, -1L, -1L);
    inpSetWaitTicks(30);  // 3

    PlayAnim("Reperatur", 30000, GFX_DONT_SHOW_FIRST_PIC);

    while (!(choice & INP_LBUTTONP) && enough && (!ready))
    {
        line = 0;

        AddVTime(3);

        AddPresentLine(presentationData, PRESENT_AS_TEXT, 0L, 0L, list, line++);

        if (item) AddPresentLine(presentationData, PRESENT_AS_BAR, (uint32_t)(*item), 255L, list, line++);

        AddPresentLine(presentationData, PRESENT_AS_BAR, (uint32_t)(car->State), 255L, list, line++);

        AddPresentLine(presentationData, PRESENT_AS_NUMBER, totalCosts, 0L, list, line++);
        AddPresentLine(presentationData, PRESENT_AS_NUMBER, tcGetPlayerMoney, 0L, list, line++);

        DrawPresent(presentationData, 0, u_wrp, (ubyte)GetNrOfNodes(presentationData));

        RemoveNode(presentationData, NULL);

        choice = inpWaitFor(INP_LBUTTONP | INP_TIME);

        if (choice & INP_TIME)
        {
            if ((enough = tcSpendMoney(costs, 1)))
            {
                totalCosts += costs;

                if (type & 1) tcSetCarMotorState(car, 1);

                if (type & 2) tcSetCarBodyState(car, 1);

                if (type & 4) tcSetCarTyreState(car, 1);
            }
        }

        if (item)
        {
            if ((*item) == 255)
            {
                StopAnim();
                gfxRefresh();
                Say(BUSINESS_TXT, 0, marc->PictID, "REP_READY");
                ready = 1;
            }
        }
        else
        {
            if (tcGetCarTotalState(car) > 253)
            {
                StopAnim();
                gfxRefresh();
                Say(BUSINESS_TXT, 0, marc->PictID, "REP_READY");
                ready = 1;
            }
        }
    }

    if (!ready)
    {
        StopAnim();
        gfxRefresh();
    }

    inpSetWaitTicks(0L);

    // gfxShow(26,GFX_NO_REFRESH|GFX_ONE_STEP,0L,-1L,-1L);
    // gfxShow((uword)car->PictID,GFX_NO_REFRESH|GFX_OVERLAY,1L,-1L,-1L);

    RemoveList(presentationData);
    RemoveList(list);
}

uint32_t tcChooseCar(uint32_t backgroundNr)
{
    LIST *bubble = NULL;
    uint32_t carCount = 0;
    uint32_t carID = 0L;
    ubyte choice = 0;
    Car matts_car = NULL;

    hasAll(Person_Matt_Stuvysunt, OLF_PRIVATE_LIST | OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Car);
    bubble = ObjectListPrivate;

    if (!(LIST_EMPTY(bubble)))
    {
        carCount = GetNrOfNodes(bubble);

        choice = 0;  // 2014-07-01 LucyG: initialize

        if (carCount == 1)
        {
            carID = OL_NR(LIST_HEAD(bubble));
        }
        else
        {
            char exp[TXT_KEY_LENGTH];

            txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", exp);
            ExpandObjectList(bubble, exp);

            Say(BUSINESS_TXT, 0, 7, "ES GEHT UM..");

            choice = Bubble(bubble, 0, 0L, 0L);
            if (ChoiceOk(choice, GET_OUT, bubble))
                carID = OL_NR(GetNthNode(bubble, (uint32_t)choice));
            else
                choice = GET_OUT;
        }

        if (choice != GET_OUT)
        {
            matts_car = (Car)dbGetObject(carID);
            SetCarColors((ubyte)matts_car->ColorIndex);
            gfxShow(backgroundNr, GFX_NO_REFRESH | GFX_ONE_STEP, 0L, -1L, -1L);
            gfxShow((uword)matts_car->PictID, GFX_NO_REFRESH | GFX_OVERLAY, 1L, -1L, -1L);
        }
    }

    RemoveList(bubble);

    return carID;
}

void tcCarGeneralOverhoul(Car car)
{
    Person marc = (Person)dbGetObject(Person_Marc_Smith);
    LIST *bubble = NULL;
    ubyte choice = 0;

    SetPictID(marc->PictID);

    bubble =
        txtGoKeyAndInsert(BUSINESS_TXT, "GENERAL_OVERHOUL", (uint32_t)((tcCostsPerTotalRepair(car) * 255) / 8), NULL);
    Bubble(bubble, 0, 0L, 0L);
    RemoveList(bubble);

    choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "GENERAL_OVERHOUL_QUEST");

    if (choice == 0) tcRepairCar(car, "TotalRepair");
}
