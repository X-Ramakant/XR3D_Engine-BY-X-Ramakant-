/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Entry point for the XR3D_UnitTests target. Runs every test registered
* via the XR3D_TEST macro across all Unit test files.

*/

#include "TestFramework.h"

int main()
{
    return XR3D::Tests::RunAllTests();
}