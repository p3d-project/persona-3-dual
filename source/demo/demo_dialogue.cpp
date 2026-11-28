#include "demo_dialogue.hpp"

// dialogue content
Dialogue demoDialogueLines[29];
Dialogue* demoDialogueInit()
{
    DialogueSelection demoDialogueSel60 = {"Vouch for Yukari", false, &demoDialogueLines[7]};
    DialogueSelection demoDialogueSel61 = {"Side with Aigis", false, &demoDialogueLines[12]};
    DialogueSelection demoDialogueSel62 = {"Ask what's outside", false, &demoDialogueLines[15]};

    DialogueSelection demoDialogueSel190 = {"Offer to walk her", false, &demoDialogueLines[20]};
    DialogueSelection demoDialogueSel191 = {"Tell her it can wait", false, &demoDialogueLines[23]};
    DialogueSelection demoDialogueSel192 = {"Follow the rule", false, &demoDialogueLines[26]};

    demoDialogueLines[0] = {"Aigis",
                            "Yukari, the\xFF\x01\x07 east path\xFF\x01\xFF is closed after sundown. "
                            "It's the second time I have stated this.",
                            BustAigis::spNeutral,
                            NULL,
                            &demoDialogueLines[1],
                            {}};
    demoDialogueLines[1] = {"Yukari",
                            "I know, I know - I just left my bag on the bench. Thirty seconds, tops.",
                            BustYukari::spNeutral,
                            &demoDialogueLines[0],
                            &demoDialogueLines[2],
                            {}};

    demoDialogueLines[2] = {"Aigis",
                            "I am not authorized to make exceptions. Please retrieve it after sunrise.",
                            BustAigis::spNeutral,
                            &demoDialogueLines[1],
                            &demoDialogueLines[3],
                            {}};
    demoDialogueLines[3] = {
        "Akihiko", "What's the holdup?", BustAkihiko::spNeutral, &demoDialogueLines[2], &demoDialogueLines[4], {}};
    demoDialogueLines[4] = {"Yukari",
                            "Aigis has decided my bag is a security threat.",
                            BustYukari::spNeutral,
                            &demoDialogueLines[3],
                            &demoDialogueLines[5],
                            {}};
    demoDialogueLines[5] = {"Aigis",
                            "That is not accurate. The bag itself is not the concern. The curfew is.",
                            BustAigis::spNeutral,
                            &demoDialogueLines[4],
                            &demoDialogueLines[6],
                            {}};
    demoDialogueLines[6] = {"Akihiko",
                            "Nobody's arguing. Let's just sort this out.",
                            BustAkihiko::spNeutral,
                            &demoDialogueLines[5],
                            NULL,
                            {demoDialogueSel60, demoDialogueSel61, demoDialogueSel62}};

    // Branch: Vouch for Yukari
    demoDialogueLines[7] = {"Akihiko",
                            "Come on, it's a bag, not a break-in. Let her grab it.",
                            BustAkihiko::spNeutral,
                            &demoDialogueLines[6],
                            &demoDialogueLines[8],
                            {}};
    demoDialogueLines[8] = {
        "Aigis", "...Processing.", BustAigis::spNeutral, &demoDialogueLines[7], &demoDialogueLines[9], {}};
    demoDialogueLines[9] = {"Aigis",
                            "Acceptable. Two minutes. Sanada, you will accompany her, since you vouched for this.",
                            BustAigis::spNeutral,
                            &demoDialogueLines[8],
                            &demoDialogueLines[10],
                            {}};
    demoDialogueLines[10] = {"Yukari",
                             "Fine by me. Thanks, Aigis.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[9],
                             &demoDialogueLines[11],
                             {}};
    demoDialogueLines[11] = {
        "Akihiko", "...Guess I'm coming along, then.", BustAkihiko::spNeutral, &demoDialogueLines[10], NULL, {}};

    // Branch: Side with Aigis
    demoDialogueLines[12] = {"Akihiko",
                             "She's right. It can wait till morning, Yukari.",
                             BustAkihiko::spNeutral,
                             &demoDialogueLines[6],
                             &demoDialogueLines[13],
                             {}};
    demoDialogueLines[13] = {"Yukari",
                             "Unbelievable. Both of you, actually.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[12],
                             &demoDialogueLines[14],
                             {}};
    demoDialogueLines[14] = {"Yukari",
                             "Don't expect me to remember this favorably.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[13],
                             NULL,
                             {}};

    // Branch: Ask what's actually out there
    demoDialogueLines[15] = {"Akihiko",
                             "Out of curiosity - what's actually out there after dark?",
                             BustAkihiko::spNeutral,
                             &demoDialogueLines[6],
                             &demoDialogueLines[16],
                             {}};
    demoDialogueLines[16] = {"Aigis",
                             "That information is outside the scope of what I am permitted to disclose.",
                             BustAigis::spNeutral,
                             &demoDialogueLines[15],
                             &demoDialogueLines[17],
                             {}};
    demoDialogueLines[17] = {"Aigis",
                             "What I can say is this: my directive is to ensure no one is on that path after sundown.",
                             BustAigis::spNeutral,
                             &demoDialogueLines[16],
                             &demoDialogueLines[18],
                             {}};
    demoDialogueLines[18] = {"Yukari",
                             "I-it's not like it's haunted or anything. It's just a rule.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[17],
                             &demoDialogueLines[19],
                             {}};
    demoDialogueLines[19] = {"Akihiko",
                             "Alright. So what do we do about the bag.",
                             BustAkihiko::spNeutral,
                             &demoDialogueLines[18],
                             NULL,
                             {demoDialogueSel190, demoDialogueSel191, demoDialogueSel192}};

    // Sub-branch: Offer to walk her there yourself
    demoDialogueLines[20] = {
        "Aigis",
        "...Acceptable. I will permit this. Return within five minutes, or I will come retrieve you both.",
        BustAigis::spNeutral,
        &demoDialogueLines[19],
        &demoDialogueLines[21],
        {}};
    demoDialogueLines[21] = {"Yukari",
                             "Thanks, Akihiko. Didn't expect that from you.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[20],
                             &demoDialogueLines[22],
                             {}};
    demoDialogueLines[22] = {"Akihiko",
                             "Don't mention it. Let's move before she changes her mind.",
                             BustAkihiko::spNeutral,
                             &demoDialogueLines[21],
                             NULL,
                             {}};

    // Sub-branch: Tell her it can wait until morning
    demoDialogueLines[23] = {"Yukari",
                             "...Fine. It's just a bag.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[19],
                             &demoDialogueLines[24],
                             {}};
    demoDialogueLines[24] = {"Aigis",
                             "A reasonable decision. Sanada's judgment is noted favorably.",
                             BustAigis::spNeutral,
                             &demoDialogueLines[23],
                             &demoDialogueLines[25],
                             {}};
    demoDialogueLines[25] = {
        "Yukari", "Don't get used to it.", BustYukari::spNeutral, &demoDialogueLines[24], NULL, {}};

    // Sub-branch: Agree the rule's there for a reason
    demoDialogueLines[26] = {"Yukari",
                             "Oh, come on. Not you too.",
                             BustYukari::spNeutral,
                             &demoDialogueLines[19],
                             &demoDialogueLines[27],
                             {}};
    demoDialogueLines[27] = {"Aigis",
                             "Agreement noted. My assessment of Sanada has improved.",
                             BustAigis::spNeutral,
                             &demoDialogueLines[26],
                             &demoDialogueLines[28],
                             {}};
    demoDialogueLines[28] = {
        "Yukari", "I'm going to bed. Alone. On purpose.", BustYukari::spNeutral, &demoDialogueLines[27], NULL, {}};

    // return the first dialogue line
    return &demoDialogueLines[0];
}
