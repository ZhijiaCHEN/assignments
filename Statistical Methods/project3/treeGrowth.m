growth = readtable('groupTreeGrowth.csv');
growth.growthGroup = nominal(growth.growthGroup);
growth.pdsiGroup = nominal(growth.pdsiGroup);

model1 = fitlm(growth,'dcm~1+cm+summerpdsi+wintertemp+(cm|growthGroup)+(summerpdsi|growthGroup)+(wintertemp|growthGroup)');
model2 = fitlme(growth,'dcm~1+cm+wintertemp+annualprec+(1|pdsiGroup)');
model3 = fitlme(growth,'dcm~1+cm+wintertemp+(1|pdsiGroup)');
model4 = fitlme(growth,'dcm~1+cm+(1|pdsiGroup)');
model5 = fitlme(growth,'dcm~1+cm+(cm-1|pdsiGroup)');
model6 = fitlme(growth,'dcm~1+cm+(cm|id)');
model7 = fitlme(growth,'dcm~1+cm+(1|id)');