#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// This is a solution of counting coins problem:
//
// Given an input array of integers with n entries and a desired target_sum,
// returns the number of combinations, of any length, that add up to
// that target_sum.
//
// Author: Xiaobo Sun
// Data  : April 25, 2014
//
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 *                         countCombH
 * Helper function (recursive)
 *
 * nums      : array of integers
 * n         : the length of nums
 * prevIdx   : the index of number that is selected in the previous iteration
 * curSum    : the current sum of numbers
 * targetSum : target sum
 *
 * returns the number of combinations
 ************************************************************************/
static int countCombH(int nums[], int n, int prevIdx, int curSum, int targetSum)
{
    long unsigned count = 0;
    int sum = 0;
    int i = 0;
    
    // do not use number that is before the previous entry
    for(i = prevIdx; i < n; i++)
    {
        sum = nums[i] + curSum;
        // increase count by 1 if reached the target sum
        if(sum == targetSum)
        {
            count++;
        }
        // count the combination recursively
        else if(sum < targetSum)
        {
            count += countCombH(nums, n, i, sum, targetSum);
        }
    }
    
    return count;
}

/************************************************************************
 *                            countComb
 * count the combination
 *
 * nums      : array of integers
 * n         : the length of nums
 ***********************************************************************/
long unsigned countComb(int numbers[], int n, int targetSum)
{
    return countCombH(numbers, n, 0, 0, targetSum);
}

/************************************************************************
 *                            main
 ***********************************************************************/
int main(int argc, char* argv[])
{
    int num1[] = { 100, 50, 25, 10, 5, 1 };
    int target1 = 1000;
    
    long unsigned result1 = countComb(num1, 4, target1);
    
    printf("the number of combination is: %lu\n", result1);
    
}