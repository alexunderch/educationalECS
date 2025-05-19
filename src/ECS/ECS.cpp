#include "ECS.h" 
#include <stdexcept>

int IComponent::nextId = 0;

int Entity::GetId() const{
    return id; //incapsulation
}

void System::AddEntityToSystem(Entity entity) {
    entities.push_back(entity);
}

void System::RemoveEntityToSystem(Entity entity){
    
    entities.erase(
        std::remove_if(
            entities.begin(), 
            entities.end(), 
            [&entity](Entity other){
                return entity==other;
            }
        ), entities.end()
    );

    //my way
    // int _id = entity.GetId();
    // for (std::vector<Entity>::iterator it = entities.begin(); it != entities.end();)
    // {
    //     if ((*it).GetId() == _id)
    //         entities.erase(it);
    //         break;
    // }
    
}

std::vector<Entity> System::GetSystemEntities() const{
    return entities;
}

const Signature& System::GetComponentSignature() const{
    return componentSignature;
}

void Registry::AddEntityToSystems(Entity entity){
    const int entityId = entity.GetId();

    //need to match entityComponentSignature <--> systemComponentSignature
    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& [sys_type, system]: systems){
        const auto& systemComponentSignature = system->GetComponentSignature();
        bool is_interested = ( //they match
            systemComponentSignature & entityComponentSignature
        ) == systemComponentSignature;

        if (is_interested) {
            system->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity){

    for (auto& [sys_type, system]: systems){
        system->RemoveEntityToSystem(entity);
    }
}

void Registry::Update(){
    //insert/delete entities AT THE END of a game loop frame
    //to avoid any confusion of any other components

    //Adding first (matching with the correct systems)
    for (auto entity: entitiesToBeAdded){
        AddEntityToSystems(entity);
    }
    //done for the current frame
    entitiesToBeAdded.clear();

    for (auto entity: entitiesToBeKilled){
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        //remove the entity from the component pools
        for (auto pool: componentPools){
            if (pool){
                pool->RemoveEntityFromPool(entity.GetId());
            }
        }

        //make the entity id to be reusable
        freeIds.push_back(entity.GetId());
        
        //remove any traces of tag/group maps
        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);

    }

    entitiesToBeKilled.clear();

}

Entity Registry::CreateEntity(){
    int entityId;

    if (freeIds.empty()) {
        //no reusable entities -> create new one
        entityId = numEntities++;
        if (entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        } 
    } else {
        // Reuse an id from the list of previously removed entities
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    
    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    
    // we need to make sure that the entityComponentSignatures can 
    //accomodate the new entity
    // if (entityId >= entityComponentSignatures.size()){
    //    entityComponentSignatures.resize(entityId + 1);
    // }
    
    Logger::Log("Entity has been created with id " + std::to_string(entityId));
    return entity;
}

void Registry::TagEntity(Entity entity, const std::string& tag) {
    enityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) const {

    if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end()) {
        return false;
    }

    return enityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntityByTag(const std::string& tag) const {
    if (enityPerTag.find(tag) == enityPerTag.end()){
        throw std::out_of_range("No such tag for the entity: " + tag);
    }
    return enityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
    auto taggedEntity = tagPerEntity.find(entity.GetId());
    if (taggedEntity != tagPerEntity.end()) {
        enityPerTag.erase(taggedEntity->second);
        tagPerEntity.erase(taggedEntity);
    }
}

void Registry::GroupEntity(Entity entity, const std::string& group) {
    enityPerGroup.emplace(group, std::set<Entity>());
    enityPerGroup.at(group).emplace(entity);
    groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string& group) const {
    auto groupedEntities = enityPerGroup.at(group);
    const auto hasTag = std::find(groupedEntities.begin(), groupedEntities.end(), entity.GetId());
    return hasTag != groupedEntities.end(); 
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const {
    if (enityPerGroup.find(group) == enityPerGroup.end()){
        throw std::out_of_range("No such group of entities: " + group);
    }
    auto& setOfEntities = enityPerGroup.at(group);
    return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
    
}

void Registry::RemoveEntityGroup(Entity entity) {
    auto groupedEntity = groupPerEntity.find(entity.GetId());
    if (groupedEntity != groupPerEntity.end()) {
        auto group = enityPerGroup.find(groupedEntity->second);
        if (group != enityPerGroup.end()) {
            auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end()) {
                group->second.erase(entityInGroup);
            }
        }
        groupPerEntity.erase(groupedEntity);
    }
}

void Entity::Kill() {
    Logger::Log("Entity has been killed with id " + std::to_string(id));
    registry->KillEntity(*this);
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
}

void Entity::Tag(const std::string& tag) {
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const {
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) {
    registry->GroupEntity(*this, group);

}
bool Entity::BelongsToGroup(const std::string& group) const {
    return registry->EntityBelongsToGroup(*this, group);
}