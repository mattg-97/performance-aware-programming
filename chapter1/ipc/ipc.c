typedef unsigned int u32;

u32 SingleScalarUnrolled2(u32 count, u32 *input) {
    u32 sum = 0;
    for (u32 index = 0; index < count; index += 2) {
        sum += input[index];
        sum += input[index + 1];
    }
    return sum;
}
u32 SingleScalarUnrolled4(u32 count, u32 *input) {
    u32 sum = 0;
    for (u32 index = 0; index < count; index += 4) {
        sum += input[index];
        sum += input[index + 1];
        sum += input[index + 2];
        sum += input[index + 3];
    }
    return sum;
}

u32 DualScalar(u32 count, u32 *input) {
    u32 sumA = 0;
    u32 sumB = 0;
    for (u32 index = 0; index < count; index += 2) {
        sumA += input[index + 0];
        sumB += input[index + 1];
    }
    u32 sum = sumA + sumB;
    return sum;
}
u32 QuadScalar(u32 count, u32 *input) {
    u32 sumA = 0;
    u32 sumB = 0;
    u32 sumC = 0;
    u32 sumD = 0;
    for (u32 index = 0; index < count; index += 4) {
        sumA += input[index + 0];
        sumB += input[index + 1];
        sumC += input[index + 2];
        sumD += input[index + 3];
    }
    u32 sum = sumA + sumB + sumC + sumD;
    return sum;
}
u32 QuadScalarPtr(u32 count, u32 *input) {
    u32 sumA = 0;
    u32 sumB = 0;
    u32 sumC = 0;
    u32 sumD = 0;
    count /= 4;
    while (count--) {
        sumA += input[0];
        sumB += input[1];
        sumC += input[2];
        sumD += input[3];
        input += 4;
    }
    u32 sum = sumA + sumB + sumC + sumD;
    return sum;
}