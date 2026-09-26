#include "demo_dialogue.hpp"

// dialogue content
Dialogue demo_dialogue_lines[29];
Dialogue* demo_dialogue_init()
{
    DialogueSelection demo_dialogue_sel_6_0 = {"Vouch for Yukari", false, &demo_dialogue_lines[7]};
    DialogueSelection demo_dialogue_sel_6_1 = {"Side with Aigis", false, &demo_dialogue_lines[12]};
    DialogueSelection demo_dialogue_sel_6_2 = {"Ask what's outside", false, &demo_dialogue_lines[15]};

    DialogueSelection demo_dialogue_sel_19_0 = {"Offer to walk her", false, &demo_dialogue_lines[20]};
    DialogueSelection demo_dialogue_sel_19_1 = {"Tell her it can wait", false, &demo_dialogue_lines[23]};
    DialogueSelection demo_dialogue_sel_19_2 = {"Follow the rule", false, &demo_dialogue_lines[26]};

    demo_dialogue_lines[0] = {"Aigis",
                              "Yukari, the\xFF\x01\x07 east path\xFF\x01\xFF is closed after sundown. "
                              "It's the second time I have stated this.",
                              BustAigis::spNeutral,
                              NULL,
                              &demo_dialogue_lines[1],
                              {}};
    demo_dialogue_lines[1] = {"Yukari",
                              "I know, I know - I just left my bag on the bench. Thirty seconds, tops.",
                              BustYukari::spNeutral,
                              &demo_dialogue_lines[0],
                              &demo_dialogue_lines[2],
                              {}};
    demo_dialogue_lines[2] = {"Aigis",
                              "I am not authorized to make exceptions. Please retrieve it after sunrise.",
                              BustAigis::spNeutral,
                              &demo_dialogue_lines[1],
                              &demo_dialogue_lines[3],
                              {}};
    demo_dialogue_lines[3] = {
        "Akihiko", "What's the holdup?", BustAkihiko::spNeutral, &demo_dialogue_lines[2], &demo_dialogue_lines[4], {}};
    demo_dialogue_lines[4] = {"Yukari",
                              "Aigis has decided my bag is a security threat.",
                              BustYukari::spNeutral,
                              &demo_dialogue_lines[3],
                              &demo_dialogue_lines[5],
                              {}};
    demo_dialogue_lines[5] = {"Aigis",
                              "That is not accurate. The bag itself is not the concern. The curfew is.",
                              BustAigis::spNeutral,
                              &demo_dialogue_lines[4],
                              &demo_dialogue_lines[6],
                              {}};
    demo_dialogue_lines[6] = {"Akihiko",
                              "Nobody's arguing. Let's just sort this out.",
                              BustAkihiko::spNeutral,
                              &demo_dialogue_lines[5],
                              NULL,
                              {demo_dialogue_sel_6_0, demo_dialogue_sel_6_1, demo_dialogue_sel_6_2}};

    // Branch: Vouch for Yukari
    demo_dialogue_lines[7] = {"Akihiko",
                              "Come on, it's a bag, not a break-in. Let her grab it.",
                              BustAkihiko::spNeutral,
                              &demo_dialogue_lines[6],
                              &demo_dialogue_lines[8],
                              {}};
    demo_dialogue_lines[8] = {
        "Aigis", "...Processing.", BustAigis::spNeutral, &demo_dialogue_lines[7], &demo_dialogue_lines[9], {}};
    demo_dialogue_lines[9] = {"Aigis",
                              "Acceptable. Two minutes. Sanada, you will accompany her, since you vouched for this.",
                              BustAigis::spNeutral,
                              &demo_dialogue_lines[8],
                              &demo_dialogue_lines[10],
                              {}};
    demo_dialogue_lines[10] = {"Yukari",
                               "Fine by me. Thanks, Aigis.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[9],
                               &demo_dialogue_lines[11],
                               {}};
    demo_dialogue_lines[11] = {
        "Akihiko", "...Guess I'm coming along, then.", BustAkihiko::spNeutral, &demo_dialogue_lines[10], NULL, {}};

    // Branch: Side with Aigis
    demo_dialogue_lines[12] = {"Akihiko",
                               "She's right. It can wait till morning, Yukari.",
                               BustAkihiko::spNeutral,
                               &demo_dialogue_lines[6],
                               &demo_dialogue_lines[13],
                               {}};
    demo_dialogue_lines[13] = {"Yukari",
                               "Unbelievable. Both of you, actually.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[12],
                               &demo_dialogue_lines[14],
                               {}};
    demo_dialogue_lines[14] = {"Yukari",
                               "Don't expect me to remember this favorably.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[13],
                               NULL,
                               {}};

    // Branch: Ask what's actually out there
    demo_dialogue_lines[15] = {"Akihiko",
                               "Out of curiosity - what's actually out there after dark?",
                               BustAkihiko::spNeutral,
                               &demo_dialogue_lines[6],
                               &demo_dialogue_lines[16],
                               {}};
    demo_dialogue_lines[16] = {"Aigis",
                               "That information is outside the scope of what I am permitted to disclose.",
                               BustAigis::spNeutral,
                               &demo_dialogue_lines[15],
                               &demo_dialogue_lines[17],
                               {}};
    demo_dialogue_lines[17] = {
        "Aigis",
        "What I can say is this: my directive is to ensure no one is on that path after sundown.",
        BustAigis::spNeutral,
        &demo_dialogue_lines[16],
        &demo_dialogue_lines[18],
        {}};
    demo_dialogue_lines[18] = {"Yukari",
                               "I-it's not like it's haunted or anything. It's just a rule.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[17],
                               &demo_dialogue_lines[19],
                               {}};
    demo_dialogue_lines[19] = {"Akihiko",
                               "Alright. So what do we do about the bag.",
                               BustAkihiko::spNeutral,
                               &demo_dialogue_lines[18],
                               NULL,
                               {demo_dialogue_sel_19_0, demo_dialogue_sel_19_1, demo_dialogue_sel_19_2}};

    // Sub-branch: Offer to walk her there yourself
    demo_dialogue_lines[20] = {
        "Aigis",
        "...Acceptable. I will permit this. Return within five minutes, or I will come retrieve you both.",
        BustAigis::spNeutral,
        &demo_dialogue_lines[19],
        &demo_dialogue_lines[21],
        {}};
    demo_dialogue_lines[21] = {"Yukari",
                               "Thanks, Akihiko. Didn't expect that from you.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[20],
                               &demo_dialogue_lines[22],
                               {}};
    demo_dialogue_lines[22] = {"Akihiko",
                               "Don't mention it. Let's move before she changes her mind.",
                               BustAkihiko::spNeutral,
                               &demo_dialogue_lines[21],
                               NULL,
                               {}};

    // Sub-branch: Tell her it can wait until morning
    demo_dialogue_lines[23] = {"Yukari",
                               "...Fine. It's just a bag.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[19],
                               &demo_dialogue_lines[24],
                               {}};
    demo_dialogue_lines[24] = {"Aigis",
                               "A reasonable decision. Sanada's judgment is noted favorably.",
                               BustAigis::spNeutral,
                               &demo_dialogue_lines[23],
                               &demo_dialogue_lines[25],
                               {}};
    demo_dialogue_lines[25] = {
        "Yukari", "Don't get used to it.", BustYukari::spNeutral, &demo_dialogue_lines[24], NULL, {}};

    // Sub-branch: Agree the rule's there for a reason
    demo_dialogue_lines[26] = {"Yukari",
                               "Oh, come on. Not you too.",
                               BustYukari::spNeutral,
                               &demo_dialogue_lines[19],
                               &demo_dialogue_lines[27],
                               {}};
    demo_dialogue_lines[27] = {"Aigis",
                               "Agreement noted. My assessment of Sanada has improved.",
                               BustAigis::spNeutral,
                               &demo_dialogue_lines[26],
                               &demo_dialogue_lines[28],
                               {}};
    demo_dialogue_lines[28] = {
        "Yukari", "I'm going to bed. Alone. On purpose.", BustYukari::spNeutral, &demo_dialogue_lines[27], NULL, {}};

    // return the first dialogue line
    return &demo_dialogue_lines[0];
}
