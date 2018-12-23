// Function to return LCM of two numbers
unsigned long  getLCM(int a, int b) {
  if (a == b) return 1;
  return (a * b) / gcd(a, b);

}

unsigned long int gcd(int a, int b)
{
  // Everything divides 0
  if (a == 0 || b == 0)
    return 0;

  // Base case
  if (a == b) return a;

  // a is greater
  if (a > b) return gcd(a - b, b);

  return gcd(a, b - a);
}



