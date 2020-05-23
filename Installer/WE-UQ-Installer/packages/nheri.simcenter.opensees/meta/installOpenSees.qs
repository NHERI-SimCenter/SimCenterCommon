function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/opensees.exe", "@StartMenuDir@/opensees.lnk", "workingDirectory=@TargetDir@", "description=OpenSees analysis program"); }
}
