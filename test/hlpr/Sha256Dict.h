#ifndef _TEST_SHA256DICT_H_
#define _TEST_SHA256DICT_H_


inline constexpr const char* sha256Dict(int i, bool sha256) {

    static constexpr const char *tags[] { "Tag00", "Tag01", "Tag02", "Tag03", "Tag04", "Tag05", "Tag06", "Tag07", "Tag08", "Tag09" };
    static constexpr const char *shas[] {
        "5b140bed7308359f187ad090703287cfa930e8aa771e740b0f1ba08301c4b657",
        "dbde9911a3f7045b80cf40162e54e7fdef225b53283ff6aaae82e09e68c5a1f7",
        "c3a2752521455264bfcf781e794b183293361b87ac4e9a0548df0c14b87d4911",
        "ffc0f165f6222a3b4c1f853b92d2eea69695ab5a1e4efbc896971dc95457d62d",
        "83e2c57a5e6964e017f6317919fb83dd6d5449c98bbe11d19116c76e913247c7",
        "c94b495504d759b7e40c38371ffc3743eac1dd0fbfbd9c23d5f96e8788dea7cc",
        "1bc34a7550d3a2809186fab31ba679aa89689a007d170e0d26cca50f76ba5708",
        "bc9126361948e12db903038e12933e616de79e0c3088030dd9ed832a3cf6288e",
        "fae281901656805e10f7cf02c329afa745e62493506bfbdcc89cf750ea7dc601",
        "1f3fbf1a2f5f46510ffe2e76119fe9d2b0e16c8499c64d8542074f89cb0e9502"
    };

    if(shas256)
        return shas[i];
    return tags[i];
}

#endif  /* _TEST_SHA256DICT_H_ */
