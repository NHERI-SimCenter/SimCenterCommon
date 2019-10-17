function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/TurbulentInflowTool.exe", "@StartMenuDir@/Turbulent Inflow Tool.lnk", "workingDirectory=@TargetDir@", "description=Start SimCenter Turbulent Inflow Tool");
    }
}
