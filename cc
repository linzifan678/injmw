import { getNetworkInfo, Network } from "@injectivelabs/networks";
import {
  MsgSend,
  PrivateKey,
  TxClient,
  TxGrpcClient,
  ChainRestAuthApi,
  createTransaction,
} from "@injectivelabs/sdk-ts";
import { BigNumberInBase } from "@injectivelabs/utils";

const network = {
  chainId: "injective-1",
  rest: "https://a.mkkmail.com",
  grpc: "https://a.mkkmail.com",
};

const mintFn = async (privateKey) => {
    try {
      const injectiveAddress = privateKey.toBech32();
      const amount = {
        amount: new BigNumberInBase(0.000001).toWei().toFixed(),
        denom: "inj",
      };
      const publicKey = privateKey.toPublicKey().toBase64();
      const accountDetails = await new ChainRestAuthApi(network.rest).fetchAccount(injectiveAddress);
      const msg = MsgSend.fromJSON({
        amount,
        srcInjectiveAddress: injectiveAddress,
        dstInjectiveAddress: injectiveAddress,
      });
  
      const { signBytes, txRaw } = createTransaction({
        message: msg,
        memo: btoa(`data:,{"p":"injrc-20","op":"mint","tick":"INJS","amt":"2000"}`),
        fee: {
          amount: [
            {
              amount: '2000000000000000',
              denom: "inj",
            },
          ],
          gas: "400000",
        },
        pubKey: publicKey,
        sequence: parseInt(accountDetails.account.base_account.sequence, 10),
        accountNumber: parseInt(accountDetails.account.base_account.account_number, 10),
        chainId: network.chainId,
      });
  
      const signature = await privateKey.sign(Buffer.from(signBytes));
      txRaw.signatures = [signature];
  
      console.log(`Transaction Hash: ${TxClient.hash(txRaw)}`);
      const txService = new TxGrpcClient(network.grpc);
      const simulationResponse = await txService.simulate(txRaw);
      console.log(`Transaction simulation response: ${JSON.stringify(simulationResponse.gasInfo)}`);
      const txResponse = await txService.broadcast(txRaw);
      console.log(txResponse);
    } catch (e) {
      await new Promise((resolve) => setTimeout(resolve, 1000));
    }
  };

const walletMint = async (mnemonic, recipientAddress) => {
  // 在这里编写您的转账逻辑
  // 使用提供的助记词和收款地址进行签名和广播
  console.log(`转账操作：从助记词 ${mnemonic} 转账到地址 ${recipientAddress}`);
  const priv = PrivateKey.fromMnemonic(mnemonic);
  await mintFn(priv);
};

const handleMint = async () => {
  const mnemonic = "lunar unknown force vocal seek humble giggle ozone fog lamp unaware column stick grass tilt speed coach gas case about green kidney seat radar";
  const recipientAddress = "inj15xr3s4aagqph50esfdp6dh0ynxl338d2m4qnrf"; // 添加收款地址

  if (!mnemonic || !recipientAddress) {
    console.log(`请输入助记词和收款地址`);
    return;
  }

  const walletMnemonics = mnemonic.split(",");
  const loopCount = 2; // 设置循环次数为2次

  for (let i = 0; i < loopCount; i++) {
    await walletMint(walletMnemonics[i % walletMnemonics.length], recipientAddress); // 使用取余操作来循环使用助记词
    await sleep(300); // 等待300毫秒
  }
};

const sleep = (ms) => {
  return new Promise((resolve) => setTimeout(resolve, ms));
};

handleMint();
