#include <vector>

int main(void)
{
	std::vector<int> v;
	std::vector<int>::const_iterator it = v.cbegin();
	return it == v.cend() ? 0 : 1;
}
