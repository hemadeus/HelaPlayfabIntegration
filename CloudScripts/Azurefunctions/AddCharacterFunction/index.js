const { v4: uuidv4 } = require("uuid");
const axios = require("axios");
const TITLE_ID = "<<SET YOUR TITLE ID HERE>>";

module.exports = async function (context, req) {
    context.log('🛠 Test Log - Function started');
    context.log(`🛠 Test Log - Function started, req: ${JSON.stringify(req.body)}`);
    const { itemId, name, catalogVersion = "Main" } = req.body.FunctionArgument;

    if (!itemId || typeof itemId !== "string") {
        return { status: 400, body: { error: "Missing or invalid 'itemId'" } };
    }

    if (!name || typeof name !== "string") {
        return { status: 400, body: { error: "Missing or invalid 'name'" } };
    }
    context.log(`🛠 Test Log - Function started", itemid:${itemId}, name: ${name}, ${catalogVersion}`);
    const entityToken = req.body.TitleAuthenticationContext.EntityToken;
    context.log(`🛠 Test Log - Function started, entityToken: ${entityToken}`);
    context.log(`🛠 Test Log - Function started, entityToken: ${entityToken}, entity: ${JSON.stringify(req.body.CallerEntityProfile)}`);
    const entity = req.body.CallerEntityProfile.Entity; // includes Id and Type

    context.log(`🛠 Test Log - Function started, entityToken: ${entityToken}, entity: ${JSON.stringify(entity)}`);
    try {
        // // Get catalog
        // const catalog = await axios.post(
        //     `https://${TITLE_ID}.playfabapi.com/Catalog/GetCatalogItems`,
        //     { CatalogVersion: catalogVersion },
        //     {
        //         headers: {
        //             "X-EntityToken": entityToken,
        //             "Content-Type": "application/json"
        //         }
        //     }
        // );
        // const catalogItem = catalog.data.Catalog.find(i => i.Id === itemId);
        // if (!catalogItem || catalogItem.ContentType !== "character") {
        //     return { status: 400, body: { error: "Item not found or not of type 'character'" } };
        // }

        // {
        //     "Entity": {
        //         "Type": "title_player_account",
        //         "Id": "6E854783F3228EB6"
        //     },
        //     "Item": {
        //         "Id": "e1c7e208-f453-4370-b645-37571515cc3b",
        //         "DisplayProperties": {
        //           "name": "Brave Jeff"
        //         }
        //     },
        //     "Amount": 1
        // }
        // Add inventory item

        const payload = {
            Entity: {
                Id: entity.Id,
                Type: entity.Type
            },
            CollectionId: "characters",
            Item: {
                Id: itemId,
            },
            Amount: 1,
            NewStackValues: {
                DisplayProperties: {
                    name: name
                }
            }
        };

        context.log(`🛠 Test Log - add result start`);
        const addResult = await axios.post(
            `https://${TITLE_ID}.playfabapi.com/Inventory/AddInventoryItems`,
            payload,
            {
                headers: {
                    "X-EntityToken": entityToken,
                    "Content-Type": "application/json"
                }
            }
        );
        context.log("✅ Success:", JSON.stringify(payload));
        return {
            status: 200,
            body: {
                success: true,
                item: payload
            }
        };
    } catch (err) {
        context.log.error("Error in Azure Function:", err);
        return {
            status: 500,
            body: {
                error: err.response?.data || err.message || "Unexpected error"
            }
        };
    }
};