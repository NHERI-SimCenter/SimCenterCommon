function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/TurbulenceInflowTool.exe", "@StartMenuDir@/Turbulence Inflow Tool.lnk", "workingDirectory=@TargetDir@", "description=Start SimCenter Turbulence Inflow Tool");
    }
}
