#ifndef BULKBUILDER_H
#define BULKBUILDER_H

#include <string>
#include <vector>

#include <TObject.h>

#include "Json.h"

class BulkBuilder : public TObject {
	private:
		std::vector<JsonObject> operations;

		void createCommand(const std::string &op, const std::string &index, const std::string &type, const std::string &id);

	public:
		BulkBuilder();
		virtual ~BulkBuilder();
		void index(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields);
		void create(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields);
		void index(const std::string &index, const std::string &type, const JsonObject &fields);
		void create(const std::string &index, const std::string &type, const JsonObject &fields);
		void update(const std::string &index, const std::string &type, const std::string &id, const JsonObject &body);
        void update_doc(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields, bool update = false);
		void del(const std::string &index, const std::string &type, const std::string &id);
		void upsert(const std::string &index, const std::string &type, const std::string &id, const JsonObject &fields);
		void clear();
		std::string str();
		bool isEmpty();
		
		ClassDef(BulkBuilder,1)
};

#endif /* BULKBUILDER_H */
