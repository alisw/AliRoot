#include "BulkBuilder.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>
#include <locale>
#include <vector>

/* ROOT's ClassImp macro for HTML doc generation */
ClassImp(BulkBuilder)

BulkBuilder::BulkBuilder() {}
BulkBuilder::~BulkBuilder() {}

void BulkBuilder::createCommand(const std::string &op, const std::string &index, const std::string &type, const std::string &id = "") {
	JsonObject command;
	JsonObject commandParams;

	if (id != "") {
		commandParams.addMemberByKey("_id", id);
	}

	commandParams.addMemberByKey("_index", index);
	commandParams.addMemberByKey("_type", type);

	command.addMemberByKey(op, commandParams);
	operations.push_back(command);
}

void BulkBuilder::index(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields) {
	createCommand("index", index, type, id);
	operations.push_back(fields);
}

void BulkBuilder::create(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields) {
	createCommand("create", index, type, id);
	operations.push_back(fields);
}

void BulkBuilder::index(const std::string &index, const std::string &type, const JsonObject &fields) {
	createCommand("index", index, type);
	operations.push_back(fields);
}

void BulkBuilder::create(const std::string &index, const std::string &type, const JsonObject &fields) {
	createCommand("create", index, type);
	operations.push_back(fields);
}

void BulkBuilder::update(const std::string &index, const std::string &type, const std::string &id, const JsonObject &body) {
    createCommand("update", index, type, id);
    operations.push_back(body);
}

void BulkBuilder::update_doc(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields, bool upsert) {
	createCommand("update", index, type, id);

	JsonObject updateFields;
	updateFields.addMemberByKey("doc", fields);
    updateFields.addMemberByKey("doc_as_upsert", upsert);

	operations.push_back(updateFields);
}

void BulkBuilder::del(const std::string &index, const std::string &type, const std::string &id) {
	createCommand("delete", index, type, id);
}

std::string BulkBuilder::str() {
	std::stringstream json;

	for(auto &operation : operations) {
		json << operation.str() << std::endl;
	}

	return json.str();
}

void BulkBuilder::upsert(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields){

  std::cerr << "BulkBuild::upsert not implemented\n";
  throw -1;
}

void BulkBuilder::clear() {
	operations.clear();
}

bool BulkBuilder::isEmpty() {
	return operations.empty();
}
