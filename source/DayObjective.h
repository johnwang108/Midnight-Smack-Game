#ifndef __DAYOBJECTIVE_H__
#define __DAYOBJECTIVE_H__

#include <cugl/cugl.h>

class DayObjective
{
private:
	int _id;
	bool _complete;
	float _requiredTime;
	float _requiredAccuracy;
	int _quantity;
	std::string _objectiveName;
	std::string _type;
	std::string _targetName;
	std::string _reward;
public:
	DayObjective();
	DayObjective(int id, std::string type);

	void init(int id, std::string type);

	int getId() { return _id; }
	void setId(int id) { _id = id; }

	bool getComplete() { return _complete; }
	void setComplete(bool comp) { _complete = comp; }

	float getReqTime() { return _requiredTime; }
	void setReqTime(float reqTime) { _requiredTime = reqTime; }

	float getReqAccuracy() { return _requiredAccuracy; }
	void setReqAccuracy(float reqAcc) { _requiredAccuracy = reqAcc; }

	int getQuantity() { return _quantity; }
	void setQuantity(int quant) { _quantity = quant; }

	std::string getName() { return _objectiveName; }
	void setName(std::string name) { _objectiveName = name; }

	std::string getType() { return _type; }
	void setString(std::string type) { _type = type; }

	std::string getTargetName() { return _targetName; }
	void setTargetName(std::string targetName) { _targetName = targetName; }

	std::string getReward() { return _reward; }
	void setReward(std::string reward) { _reward = reward; }
};

#endif // __DAYOBJECTIVE_H__
