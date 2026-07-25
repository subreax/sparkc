#include "sparkc/frontend/Frontend.h"

AstProgItem* Frontend::processNextItem() {
    auto* item = parser.parseNextProgItem();
    semantic.process(item);
    return item;
}

AstProgram* Frontend::processFullSource() {
    std::vector<AstProgItem*> items;
    while (hasNext()) {
        items.emplace_back(processNextItem());
    }
    return astFactory.program(items);
}