typedef unsigned int u32;

u32 SingleScalar(u32 Count, u32 *Input) {
    u32 sum = 0;
    for (u32 index = 0; index < Count; ++index) {
        sum += Input[index];
    }
    return sum;
}