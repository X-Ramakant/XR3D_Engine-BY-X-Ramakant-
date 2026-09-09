/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Entry point for the XR3D_Integration target. Runs every scenario
* registered via the XR3D_SCENARIO macro across all Scenarios files.

*/

#include "IntegrationTestFramework.h"

int main()
{
    return XR3D::Integration::RunAllTests();
}