#ifndef ECS_H
#define ECS_H

#include <vector>
#include <unordered_map>
#include <set>
#include <deque>
#include <typeindex>
#include <bitset>
#include <memory>
#include <algorithm>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

//we use a bitset (1s/0s) to keep track os which components an entity has
//and also keep track of which entites a system is interested in
typedef std::bitset<MAX_COMPONENTS> Signature;

class Entity {
    private: 
        int id;
    
        public:
            Entity(int id): id(id) {}
            int GetId() const;
            void Kill();

            Entity& operator = (const Entity& other) = default;

            bool operator ==(const Entity& other) const {
                return id==other.id;
            }
            bool operator !=(const Entity& other) const {
                return id!=other.id;
            }
            bool operator <(const Entity& other) const {
                return id<other.id;
            }
            bool operator >(const Entity& other) const {
                return id>other.id;
            }

            //forward declaration: the class itself is defined below
            //WARNING: cyclic pointer
            class Registry* registry;

            //Shortcuts
            template<typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
            template<typename TComponent> void RemoveComponent();
            template<typename TComponent> bool HasComponent() const;
            template<typename TComponent> TComponent& GetComponent() const;

            //Grouping and tagging
            void Tag(const std::string& tag);
            bool HasTag(const std::string& tag) const;
            void Group(const std::string& group);
            bool BelongsToGroup(const std::string& group) const;
};

struct IComponent{ //a la interface
    protected:
        static int nextId; //static = being declared only once
};

template <typename T> 
class Component: public IComponent {
    private:
        int id;

    public:
        //The unique ID for a component type of Component<T>
        static int GetId() {
            static auto id  = nextId++;
            return id;
        }

};

class System {
    private:
        //which components an entity must have for the system consider
        //the entity
        Signature componentSignature;

        //List of all entities the system is interested in
        std::vector<Entity> entities; 
    
    public:
        System() = default;
        ~System() = default;

        void AddEntityToSystem(Entity entity);
        void RemoveEntityToSystem(Entity entity);

        std::vector<Entity> GetSystemEntities() const;
        const Signature& GetComponentSignature() const;

        template <typename TComponent> void RequireComponent();
};


class IPool {
    public:
        //forces the type to be specified
        virtual ~IPool()=default;
        virtual void RemoveEntityFromPool(size_t entityId)=0; //pure virtual method
};

//Pool is basically a vector (contiguous data) of type T (component's Type)
template <typename T>
class Pool: public IPool {
    private:
        //keeping track of the vector's data 
        std::vector<T> data;
        size_t size;

        //keeping track of the packed data
        std::unordered_map<size_t, size_t> entityIdToIndex;
        std::unordered_map<size_t, size_t> indexToEntityId;
    
    public:
        Pool (size_t capacity = 100) {
            data.resize(capacity);
            size=0;
        }

        virtual ~Pool() = default;

        bool IsEmpty() const {
            return size==0;
        }

        int GetSize() const {
            return size;
        }

        void Resize(size_t n) {
            data.resize(n);
        }

        void Clear() {
            data.clear();
            entityIdToIndex.clear();
            indexToEntityId.clear();
            size=0;
        }

        // void Add(T object){
        //     data.push_back(object);
        // }

        void Remove(size_t entityId) {

        ////  How to remove by object:        
        //     data.erase(
        //         std::remove_if(
        //             data.begin(), 
        //             data.end(), 
        //             [&object](T other) {return object == other;}
        //         ), 
        //         data.end()
        //     );

            size_t indexToBeRemoved = entityIdToIndex[entityId];
            size_t lastIndex = size - 1;

            //Copying the last element to the position we attempt to remove from
            data[indexToBeRemoved]=data[lastIndex];

            size_t lastEntityId = indexToEntityId[lastIndex];

            //Updating the map to point for correct indices
            entityIdToIndex[lastEntityId]=indexToBeRemoved;
            indexToEntityId[indexToBeRemoved]=lastEntityId;

            entityIdToIndex.erase(entityId);
            indexToEntityId.erase(lastIndex);

            size--;

        }

        void Set(size_t entityId, T object) {
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
                //if the entity id exists, just replace the data    
                size_t index = entityIdToIndex[entityId];
                data[index]=object;
            } else {
                size_t index = size;
                //tracking mapping and reverse mapping of the index: entityId mapping
                entityIdToIndex.emplace(entityId, index);
                indexToEntityId.emplace(index, entityId);

                if (index >= data.capacity()) {
                    //resizing the vector if necessary
                    data.resize(size * 2);
                }
                data[index] = object;
                size++;
            }
        }

        void RemoveEntityFromPool(size_t entityId) override {
            if (entityIdToIndex.find(entityId)!=entityIdToIndex.end()){
                Remove(entityId);
            }
        }

        T& Get(size_t entityId) {
            size_t index = entityIdToIndex[entityId];
            return static_cast<T&>(data[index]); 
        } 

        T& operator [] (size_t index) {
            return static_cast<T&>(data[index]); 
        }

        const T& operator [] (size_t index) const {
            return static_cast<const T&>(data[index]); 
        }
};

class Registry {
    //Manages the creation and destuction of entities and components
    private:
        //Entity tags -- one name tag per the entity
        std::unordered_map<std::string, Entity> enityPerTag;
        std::unordered_map<int, std::string> tagPerEntity;

        //Entity groups -- one tag per a set of entities
        std::unordered_map<std::string, std::set<Entity>> enityPerGroup;
        std::unordered_map<int, std::string> groupPerEntity;

        int numEntities = 0; //number of active entities

        //temporaty containers that save entities until the end of
        //the game loop pass (Registry::Update)
        std::set<Entity> entitiesToBeAdded;
        std::set<Entity> entitiesToBeKilled;

        //each pool contains all the data for a certain component type
        //std::vector's index = id of the component type
        //pool's index = enetity id
        std::vector<std::shared_ptr<IPool>> componentPools;
        //component signatures per entity ("on" components per entity)
        //index = entity id
        std::vector<Signature> entityComponentSignatures;
        //type_index = index for a C++ type
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        //bi-list of ids of those entities that we previously deleted
        //to reuse them
        std::deque<int> freeIds;
        
    public:
        Registry() = default;

        //Entitty management
        Entity CreateEntity();
        void KillEntity(Entity entity);

        //check if the component's siganture matches
        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);

        //component management
        template<typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template<typename TComponent> void RemoveComponent(Entity entity);
        template<typename TComponent> bool HasComponent(Entity entity) const;
        template<typename TComponent> TComponent& GetComponent(Entity entity) const;

        void Update();
        
        //entity management
        template<typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template<typename TSystem> void RemoveSystem();
        template<typename TSystem> bool HasSystem() const;
        template<typename TSystem> TSystem& GetSystem() const;

        //tag management
        void TagEntity(Entity entity, const std::string& tag);
        bool EntityHasTag(Entity entity, const std::string& tag) const;
        Entity GetEntityByTag(const std::string& tag) const;
        void RemoveEntityTag(Entity entity);

        //group management
        void GroupEntity(Entity entity, const std::string& group);
        bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
        std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
        void RemoveEntityGroup(Entity entity);

};

template<typename TComponent> 
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template<typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...); //no leak, shared between the components!
    systems.insert(
        std::make_pair(std::type_index(typeid(TSystem)), newSystem)
    );
}

template<typename TSystem> 
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    if (system != systems.end()) {
        systems.erase(system);
    }
}

template<typename TSystem> 
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem> 
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}


template<typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args){
    //getting component's and entity's IDs
    const int componentId = Component<TComponent>::GetId(); //unique id for the component type
    const int entityId = entity.GetId();

    //accomodating the members of the class
    if (componentId >= static_cast<int>(componentPools.size())) {
        // componentPools.push_back(componentId+1);
        //linear overhead!
        componentPools.resize(componentId+1, nullptr);

    }

    //no any pool corresponds to the componentId
    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    //don't need this anymore as we use packed component pools
    // if  (entityId >= static_cast<int>(componentPool->GetSize())) {
    //     //accomodating the component pool for the necessary entities;
    //     componentPool->Resize(numEntities);
    // }
    
    //overall result, a new component
    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);
    //"turn on" the component for the component ids
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
    //Logger::Log("For the component id  " + std::to_string(componentId) + " --> POOL SIZE is " + std::to_string(componentPool->GetSize()));

}

template<typename TComponent> 
void Registry::RemoveComponent(Entity entity) {
    const size_t componentId = Component<TComponent>::GetId(); 
    const size_t entityId = entity.GetId();
    
    //Remove from the componentPool?
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);

    //turn the component signature "off"
    entityComponentSignatures[entityId].set(componentId, false);
    Logger::Log("Component " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
    
}

template<typename TComponent> 
bool Registry::HasComponent(Entity entity) const {
    const int componentId = Component<TComponent>::GetId(); 
    const int entityId = entity.GetId();

    if (componentId >= static_cast<int>(componentPools.size())) {
        return false;
    }

    if (entityId >= static_cast<int>(entityComponentSignatures.size())) {
        return false;
    }

    return entityComponentSignatures[entityId].test(componentId);

}

template<typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const {
    const int componentId = Component<TComponent>::GetId(); 
    const int entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);

}

template<typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args){
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template<typename TComponent> 
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template<typename TComponent> 
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template<typename TComponent> 
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

#endif